
#include "ppcdisasm/ppc-dis.hpp"

#include "ppc2cpp/common/endian.h"
#include "ppc2cpp/data_flow/FlowContext.hpp"
#include "ppc2cpp/data_flow/DataFlowAnalysis.hpp"
#include "ppc2cpp/data_flow/PpcInstructionUD.hpp"

using namespace ppc2cpp;
using namespace ppcdisasm;

void DataFlowAnalysis::functionDFA(Function& func) {
  func.dfg.blockContexts.resize(func.cfg.blocks.size());
  disassemble_init_powerpc();
  initInstructionUD();
  functionDFA(func, 0);
}

void DataFlowAnalysis::functionDFA(Function& func, uint32_t blockIdx) {
  ppc_cpu_t dialect_raw = ppc_750cl_dialect | PPC_OPCODE_RAW; // raw mode: iterate all operands and only use base mnemonics

  uint32_t* funcPtr = (uint32_t*) programLoader->getBufferAtLocation(func.location).value();
  for (uint32_t pc = func.cfg.blocks[blockIdx].start; pc < func.cfg.blocks[blockIdx].end; pc++) {
    const uint32_t insn = be32(*(funcPtr + pc));

    std::cout << "L:" << pc << "  ";
    cout_insn_powerpc(insn, std::cout, dialect_raw);
    std::cout << "\n";

    std::vector<VarnodePtr> insnInputs;
    std::vector<VarnodePtr> insnOutputs;
    FlowContext& flowContext = func.dfg.blockContexts[blockIdx]; // TODO: FlowContext is the union of output FlowContext of preceding nodes

    const struct powerpc_opcode* opcode = lookup_powerpc(insn, dialect_raw);
    InstructionUD insnUD = instructionUD(insn, opcode, dialect_raw);
    const ppc_opindex_t *opindex; // index into powerpc_operands array
    const struct powerpc_operand *operand;

    for (int i = 0; i < insnUD.inputs.size(); i++) {
      operand = powerpc_operands + insnUD.inputs[i];
      int64_t value = operand_value_powerpc (operand, insn, dialect_raw);

      if (isImmediate(operand->flags, value)) {
        VarnodePtr opVar = std::make_shared<Operandnode>(pc, i, false);
        insnInputs.push_back(opVar);
      } else { // register
        std::vector<VarnodePtr> opVars = flowContext.getDefinition(operand, value);
        if (opVars.empty()) { // register undefined, create new definition
          VarnodePtr opVar = std::make_shared<Operandnode>(pc, i, false);
          insnInputs.push_back(opVar);
        } else if (opVars.size() == 1) {
          insnInputs.push_back(opVars[0]);
        } else { // multiple potential definitions from multiple basic blocks, create phi node
          VarnodePtr opVar = std::make_shared<Phinode>(opVars);
          insnInputs.push_back(opVar);
        }
      }
    }
    
    for (int i = 0; i < insnUD.outputs.size(); i++) {
      operand = powerpc_operands + insnUD.outputs[i];
      int64_t value = operand_value_powerpc (operand, insn, dialect_raw);
          
      VarnodePtr opVar = std::make_shared<Operandnode>(pc, i, true);
      insnOutputs.push_back(opVar);
      flowContext.setDefinition(operand, value, opVar);
    }


    for (VarnodePtr output : insnOutputs) {
      output->inputs = insnInputs;
    }
  }

  // TODO: succeding block DFA
}