
#include "ppcdisasm/ppc-dis.hpp"

#include "ppc2cpp/common/endian.h"
#include "ppc2cpp/analysis/ProgramComparator.hpp"

using namespace ppcdisasm;

namespace ppc2cpp {
bool ProgramComparator::compareVarNodes(const Function& func1, const Function& func2, const VarNodePtr& var1, const VarNodePtr& var2) {
  ppc_cpu_t dialect = ppc_750cl_dialect;

  if (!dynamic_pointer_cast<DataFlowNode>(var1) || !dynamic_pointer_cast<DataFlowNode>(var2)) {
      throw std::runtime_error("Error: non dataflow nodes detected in data flow graph");
  }
  if (typeid(*var1) != typeid(*var2)) {
    std::cout << "Data flow nodes are not of the same type, " << typeid(*var1).name() << " != " << typeid(*var2).name() << "\n";
    return false;
  }

  // TODO: cache calculation of whether two nodes are equivalent somehow

  if (dynamic_pointer_cast<SinkNode>(var1) || dynamic_pointer_cast<ResultNode>(var1)) {
    DataFlowNodePtr dfn1 = dynamic_pointer_cast<DataFlowNode>(var1);
    DataFlowNodePtr dfn2 = dynamic_pointer_cast<DataFlowNode>(var2);

    int insnIdx1 = dfn1->index;
    int insnIdx2 = dfn2->index;

    uint32_t insn1 = be32(*(this->func1ptr + insnIdx1));
    uint32_t insn2 = be32(*(this->func2ptr + insnIdx2));

    const struct powerpc_opcode* opcode1 = lookup_powerpc(insn1, dialect);
    const struct powerpc_opcode* opcode2 = lookup_powerpc(insn2, dialect);

    if (opcode1 != opcode2) {
      std::cout << "Instruction mismatch: " << opcode1->name << " != " << opcode2->name << "\n";
      return false;
    }

    // instructions must belong to the same basic block
    int cfgIdx1 = func1.cfg.findBlockByInsnIdx(insnIdx1);
    int cfgIdx2 = func2.cfg.findBlockByInsnIdx(insnIdx2);
    if (cfgIdx1 != cfgIdx2 || cfgIdx1 == -1) {
      std::cout << "Instructions do not belong to the same basic block: " << cfgIdx1 << " != " << cfgIdx2 << "\n";
      return false;
    }
  } else if (dynamic_pointer_cast<FunctionInputNode>(var1)) {
    FunctionInputNodePtr n1 = dynamic_pointer_cast<FunctionInputNode>(var1);
    FunctionInputNodePtr n2 = dynamic_pointer_cast<FunctionInputNode>(var2);

    if (n1->cpuMemoryLocation != n2->cpuMemoryLocation) {
      std::cout << "Function input mismatch: " << cpuMemorySpace2label(n1->cpuMemoryLocation) << " != " << cpuMemorySpace2label(n2->cpuMemoryLocation) << "\n";
      return false;
    }
  } else if (dynamic_pointer_cast<PhiNode>(var1)) {
    // no checks for now
  } else if (dynamic_pointer_cast<CallReturnNode>(var1)) {
    // no checks for now
  } else throw std::runtime_error("Varnode type " + std::string(typeid(*var1).name()) + " does not belong to a flow graph");

  for (int inputIdx = 0; inputIdx < var1->inputs.size(); inputIdx++) {
    if (inputIdx >= var2->inputs.size()) {
      std::cout << "Expected existence of input " + std::to_string(inputIdx) + " in second variable" << "\n";
      return false;
    }

    if (!compareVarNodes(func1, func2, var1->inputs[inputIdx], var2->inputs[inputIdx])) return false;
  }
  if (var2->inputs.size() > var1->inputs.size()) {
    std::cout << "Expected existence of input " << var1->inputs.size() << " in first variable" << "\n";
    return false;
  }

  return true;
}

std::string vectorToString(const std::vector<uint32_t>& vec) {
    std::stringstream ss;
    ss << "[";
    for (const auto& num : vec) {
        ss << num << " ";
    }
    ss << "]";
    std::string result = ss.str();
    result.pop_back();  // Remove the trailing space
    return result;
}

bool ProgramComparator::compareFunctionFlows(const Function& func1, const Function& func2) {
  const ControlFlowGraph& cfg1 = func1.cfg;
  const ControlFlowGraph& cfg2 = func2.cfg;
  const DataFlowGraph& dfg1 = func1.dfg;
  const DataFlowGraph& dfg2 = func2.dfg;
  this->func1ptr = (uint32_t*) pLoader1->getBufferAtLocation(func1.location).value();
  this->func2ptr = (uint32_t*) pLoader2->getBufferAtLocation(func2.location).value();

  // TODO: More helpful non-equivalence messages, and in the "order of occurence" as much as possible.
  // Maybe come up with a system where the types of differences are aggregates and reported in an organized manner

  // basic block equivalence checking
  for (int bblockIdx = 0; bblockIdx < cfg1.blocks.size(); bblockIdx++) {
    if (bblockIdx >= cfg2.blocks.size()) {
      std::cout << "Expected existence of basic block " + std::to_string(bblockIdx) + " in second function" << "\n";
      return false;
    }

    const std::vector<SinkNodePtr>& sinks1 = dfg1.sinks[bblockIdx];
    const std::vector<SinkNodePtr>& sinks2 = dfg2.sinks[bblockIdx];

    for (int sinkIdx = 0; sinkIdx < sinks1.size(); sinkIdx++) {
      if (sinkIdx >= sinks2.size()) {
        std::cout << "Expected existence of sink " + std::to_string(bblockIdx) + " in second function" << "\n";
        return false;
      }

      const SinkNodePtr& sink1 = sinks1[sinkIdx];
      const SinkNodePtr& sink2 = sinks2[sinkIdx];
    }
    if (sinks2.size() > sinks1.size()) {
      std::cout << "Expected existence of sink " + std::to_string(bblockIdx) + " in first function" << "\n";
      return false;
    }

    // basic blocks must branch to equivalent successors
    const BasicBlock& bblock1 = cfg1.blocks[bblockIdx];
    const BasicBlock& bblock2 = cfg2.blocks[bblockIdx];
    if (cfg1.psTable[bblockIdx].succ.size() != cfg2.psTable[bblockIdx].succ.size()) {
      std::cout << "Basic block " << bblockIdx << " has unequal number of successors" <<
        cfg1.psTable[bblockIdx].succ.size() << " != " << cfg2.psTable[bblockIdx].succ.size() << "\n";
        return false;
    }
    for (int succIdx = 0; succIdx < cfg1.psTable[bblockIdx].succ.size(); succIdx++) {
      if (cfg1.psTable[bblockIdx].succ[succIdx] != cfg2.psTable[bblockIdx].succ[succIdx]) {
        std::cout << "Non equivalent successor indices " << vectorToString(cfg1.psTable[bblockIdx].succ) <<
          " != " << vectorToString(cfg2.psTable[bblockIdx].succ) << "\n";
        return false;
      }
    }
  }
  if (func2.cfg.blocks.size() > func1.cfg.blocks.size()) {
    std::cout << "Expected existence of basic block " + std::to_string(func1.cfg.blocks.size()) + " in first function" << "\n";
    return false;
  }

  return true;
}
}