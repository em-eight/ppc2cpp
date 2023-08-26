
#include "ppcdisasm/ppc-dis.hpp"

#include "ppc2cpp/common/endian.h"
#include "ppc2cpp/common/insn_properties.h"
#include "ppc2cpp/data_flow/FlowContext.hpp"
#include "ppc2cpp/data_flow/DataFlowAnalysis.hpp"
#include "ppc2cpp/data_flow/PpcInstructionUD.hpp"
#include "ppc2cpp/data_flow/PpcCallingConvention.hpp"

using namespace ppc2cpp;
using namespace ppcdisasm;

void DataFlowAnalysis::initWorklist(const Function& func) {
  worklist.clear();
  initWorklistRecurse(func, 0);
}

void DataFlowAnalysis::initWorklistRecurse(const Function& func, int blockIdx) {
  worklist.push_back(blockIdx);
  for (auto successor : func.cfg.psTable[blockIdx].succ) {
    if (func.cfg.blocks[successor].blockType == INTERNAL && std::find(worklist.begin(), worklist.end(), successor) == worklist.end()) {
      initWorklistRecurse(func, successor);
    }
  }
}

void DataFlowAnalysis::functionDFA(Function& func) {
  initWorklist(func);
  func.dfg.blockContexts.resize(func.cfg.blocks.size());
  func.dfg.sinks.resize(func.cfg.blocks.size());
  disassemble_init_powerpc();
  initInstructionUD();

  functionDFAImpl(func);
}

void initBlockContext(Function& func, uint32_t blockIdx) {
  // A block's initial flowContext is the union of the output FlowContext of preceding nodes
  FlowContext flowContext;
  for (const auto& pred_block_idx : func.cfg.psTable[blockIdx].pred) {
    flowContext.contextUnion(func.dfg.blockContexts[pred_block_idx]);
  }
  func.dfg.blockContexts[blockIdx] = flowContext;
}

void DataFlowAnalysis::functionDFAImpl(Function& func) {
  if (worklist.empty()) return;
  int blockIdx = worklist[0];
  worklist.erase(worklist.begin());

  ppc_cpu_t dialect_raw = ppc_750cl_dialect | PPC_OPCODE_RAW; // raw mode: iterate all operands and only use base mnemonics

  const FlowContextSizes prevSizes = func.dfg.blockContexts[blockIdx].getSizes();
  initBlockContext(func, blockIdx);
  FlowContext& flowContext = func.dfg.blockContexts[blockIdx];
  std::vector<SinkNodePtr>& blockSinks = func.dfg.sinks[blockIdx];
  blockSinks.clear();

  uint32_t* funcPtr = (uint32_t*) programLoader->getBufferAtLocation(func.location).value();
  for (uint32_t pc = func.cfg.blocks[blockIdx].start; pc <= func.cfg.blocks[blockIdx].end; pc++) {
    const uint32_t insn = be32(*(funcPtr + pc));

#ifndef NDEBUG
    std::cout << "L:" << pc << "  ";
    cout_insn_powerpc(insn, std::cout, dialect_raw);
    std::cout << "\n";
#endif

    std::vector<VarNodePtr> insnInputs;
    std::vector<VarNodePtr> insnOutputs;

    const struct powerpc_opcode* opcode = lookup_powerpc(insn, dialect_raw);
    InstructionUD insnUD = instructionUD(insn, opcode, dialect_raw);
    const ppc_opindex_t *opindex; // index into powerpc_operands array
    const struct powerpc_operand *operand;

    for (int i = 0; i < insnUD.imms.size(); i++) {
      VarNodePtr opVar = std::make_shared<ImmediateNode>(pc, i, insnUD.imms[i]);
      insnInputs.push_back(opVar);
    }

    for (int i = 0; i < insnUD.inputs.size(); i++) {
      Definition& opVars = flowContext.getDefinition(insnUD.inputs[i]);
      if (opVars.empty()) { // register undefined, create new definition
        VarNodePtr opVar = std::make_shared<FunctionInputNode>(pc, insnUD.inputs[i]);
        insnInputs.push_back(opVar);
      } else if (opVars.size() == 1) {
        insnInputs.insert(insnInputs.end(), opVars.begin(), opVars.end());
      } else { // multiple potential definitions from multiple basic blocks, create phi node
        VarNodePtr opVar = std::make_shared<PhiNode>(pc, insnUD.inputs[i]);
        opVar->inputs.insert(opVar->inputs.end(), opVars.begin(), opVars.end());
        insnInputs.push_back(opVar);
      }
    }
    
    for (int i = 0; i < insnUD.outputs.size(); i++) {
      VarNodePtr opVar = std::make_shared<ResultNode>(pc, i, insnUD.outputs[i]);
      insnOutputs.push_back(opVar);
      flowContext.setDefinition(insnUD.outputs[i], opVar);
    }

    // keep track of sinks
    if (storeInsn.contains(opcode->opcode)) { // stores
      SinkNodePtr sinkVar = std::make_shared<SinkNode>(pc);
      sinkVar->inputs = insnInputs;
      blockSinks.push_back(sinkVar);
    } else if (isBranch(insn)) { // branches
      SinkNodePtr sinkVar = std::make_shared<SinkNode>(pc);

      if (func.cfg.blocks[blockIdx].endsInCall) {
        // use args
        for (const CpuMemoryLocation& registerInput : callInputs) {
          Definition& opVars = flowContext.getDefinition(registerInput);
          // it is actually impossible to determine which parameters a function is using before analyzing it first.
          // For now only mark the defined registers as used. It doesn't break quivalence checking anyways
          if (opVars.empty()) { // register undefined. May still be a valid argument being bassed through from the caller's arguments
            //VarNodePtr opVar = std::make_shared<FunctionInputNode>(pc, registerInput);
            //insnInputs.push_back(opVar);
          } else if (opVars.size() == 1) {
            insnInputs.insert(insnInputs.end(), opVars.begin(), opVars.end());
          } else { // multiple potential definitions from multiple basic blocks, create phi node
            VarNodePtr opVar = std::make_shared<PhiNode>(pc, registerInput);
            opVar->inputs.insert(opVar->inputs.end(), opVars.begin(), opVars.end());
            insnInputs.push_back(opVar);
          }
        }

        // kill volatile (set to null)
        for (const CpuMemoryLocation& registerInput : killedByCall) {
          flowContext.killDefinition(registerInput);
        }

        // define return value
        for (const CpuMemoryLocation& registerInput : definedByCall) {
          VarNodePtr returnVar = std::make_shared<CallReturnNode>(pc, registerInput);
          insnOutputs.push_back(returnVar);
          flowContext.setDefinition(registerInput, returnVar);
        }
      }
      sinkVar->inputs = insnInputs;
      blockSinks.push_back(sinkVar);
    }

    for (VarNodePtr output : insnOutputs) {
      output->inputs = insnInputs;
    }
  }
   
  // exit block return value detection
  if (func.cfg.blocks[blockIdx].isExit) {
    for (const CpuMemoryLocation& returnLoc : definedByCall) {
      const Definition& returnDef = flowContext.getDefinition(returnLoc);
      if (returnDef.size() == 1) {
        ReturnNodePtr returnNode = std::make_shared<ReturnNode>(func.length(), returnLoc);
        returnNode->inputs.insert(returnNode->inputs.end(), returnDef.begin(), returnDef.end());
        blockSinks.push_back(returnNode);
      } else if (returnDef.size() > 1) { // multiple potential definitions from multiple basic blocks, create phi node
        VarNodePtr opVar = std::make_shared<PhiNode>(func.length(), returnLoc);
        opVar->inputs.insert(opVar->inputs.end(), returnDef.begin(), returnDef.end());
        ReturnNodePtr returnNode = std::make_shared<ReturnNode>(func.length(), returnLoc);
        returnNode->inputs.push_back(opVar);
        blockSinks.push_back(returnNode);
      }
    }
  }

  // If block DFA produced any changes, the block's successors need updating
  const FlowContextSizes postSizes = flowContext.getSizes();
  if (prevSizes != postSizes) {
    for (auto successor : func.cfg.psTable[blockIdx].succ) {
      if (func.cfg.blocks[successor].blockType == INTERNAL && std::find(worklist.begin(), worklist.end(), successor) == worklist.end()) {
        worklist.insert(worklist.begin(), successor);
      }
    }
  }
  functionDFAImpl(func);
}