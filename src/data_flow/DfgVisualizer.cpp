
#include <memory>

#include "ppcdisasm/ppc-dis.hpp"

#include "ppc2cpp/data_flow/DfgVisualizer.hpp"
#include "ppc2cpp/common/endian.h"
#include "ppc2cpp/data_flow/DfgVisualizer.hpp"

using namespace ppcdisasm;

namespace ppc2cpp {
#ifdef __clang__
// clang doesn't like typeid(*ptr)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpotentially-evaluated-expression"
#endif
std::string getDotLabel(const ProgramLoaderPtr& programLoader, const Function& func, const VarNodePtr& var) {
  if (DataFlowNodePtr flowVar = dynamic_pointer_cast<DataFlowNode>(var)) {
    uint32_t* funcPtr = (uint32_t*) programLoader->getBufferAtLocation(func.location).value();
    const uint32_t insn = be32(*(funcPtr + flowVar->index));
    const struct powerpc_opcode* opcode = ppcdisasm::lookup_powerpc(insn, ppcdisasm::ppc_750cl_dialect);

    if (ResultNodePtr regIn = dynamic_pointer_cast<ResultNode>(var)) {
      return opcode->name;
    } else if (CallReturnNodePtr callOut = dynamic_pointer_cast<CallReturnNode>(var)) {
      return "ret" + std::to_string(callOut->index) + cpuMemorySpace2label(callOut->cpuMemoryLocation);
    } else if (ReturnNodePtr retVar = dynamic_pointer_cast<ReturnNode>(var)) {
      return "return " + cpuMemorySpace2label(retVar->cpuMemoryLocation);
    } else if (SinkNodePtr sink = dynamic_pointer_cast<SinkNode>(var)) {
      return opcode->name;
    } else if (ImmediateNodePtr imm = dynamic_pointer_cast<ImmediateNode>(var)) {
      return std::to_string(imm->value);
    } else if (ReferenceNodePtr ref = dynamic_pointer_cast<ReferenceNode>(var)) {
      std::optional<Symbol> maybeRelocSym = programLoader->symtab.lookupByLocation(ref->reloc.destination);
      if (!maybeRelocSym) throw std::runtime_error("Could not resolve reference at " + 
        programLoader->locationString(ref->reloc.source) + " to " + programLoader->locationString(ref->reloc.destination));
      return maybeRelocSym.value().name;
    } else if (FunctionInputNodePtr registerVar = dynamic_pointer_cast<FunctionInputNode>(var)) {
      return cpuMemorySpace2label(registerVar->cpuMemoryLocation);
    } else if (PhiNodePtr phi = dynamic_pointer_cast<PhiNode>(var)) {
      return "Φ";
    } else throw std::runtime_error("Varnode type " + std::string(typeid(*var).name()) + " does not belong to a flow graph");
  } else throw std::runtime_error("Varnode type " + std::string(typeid(*var).name()) + " does not belong to a flow graph");
}

// A string that uniquely identifies a node in a graph
std::string getNodeId(const VarNodePtr& var) {
  if (DataFlowNodePtr flowVar = dynamic_pointer_cast<DataFlowNode>(var)) {
    if (ResultNodePtr resVar = dynamic_pointer_cast<ResultNode>(var)) {
      return "res_insn" + std::to_string(resVar->index) + "op" + std::to_string(resVar->index);
    } else if (ReturnNodePtr retVar = dynamic_pointer_cast<ReturnNode>(var)) {
      return "return_" + cpuMemorySpace2label(retVar->cpuMemoryLocation);
    } else if (SinkNodePtr sink = dynamic_pointer_cast<SinkNode>(var)) {
      return "sink_insn" + std::to_string(sink->index);
    } else if (CallReturnNodePtr callOut = dynamic_pointer_cast<CallReturnNode>(var)) {
      return "callret_insn" + std::to_string(callOut->index) + "op" + cpuMemorySpace2label(callOut->cpuMemoryLocation);
    } else if (PhiNodePtr phiIn = dynamic_pointer_cast<PhiNode>(var)) {
      return "phi_insn" + std::to_string(phiIn->index) + "op" + std::to_string(phiIn->index);
    } else if (ImmediateNodePtr imm = dynamic_pointer_cast<ImmediateNode>(var)) {
      return "imm_insn" + std::to_string(imm->index) + "op" + std::to_string(imm->index);
    } else if (ReferenceNodePtr ref = dynamic_pointer_cast<ReferenceNode>(var)) {
      return "ref_insn" + std::to_string(ref->index) + "op" + std::to_string(ref->index);
    } else if (FunctionInputNodePtr registerVar = dynamic_pointer_cast<FunctionInputNode>(var)) {
      return "funcinput_insn" + std::to_string(registerVar->index) + cpuMemorySpace2label(registerVar->cpuMemoryLocation);
    } else if (PhiNodePtr phiVar = dynamic_pointer_cast<PhiNode>(var)) {
      std::string id = "PHI_";
      for (const auto& input : phiVar->inputs) id += getNodeId(input);
      return id;
    } else throw std::runtime_error("Varnode type " + std::string(typeid(*var).name()) + " does not belong to a flow graph");
  } else throw std::runtime_error("Varnode type " + std::string(typeid(*var).name()) + " does not belong to a flow graph");
}
#ifdef __clang__
#pragma clang diagnostic pop
#endif

void outputNode(std::ostream& os, const ProgramLoaderPtr& programLoader, const Function& func, const VarNodePtr& var) {
  os << "  "; // indentation
  os << getNodeId(var) << "[label=\"" + getDotLabel(programLoader, func, var) + "\"]" << ";\n";
}

void outputDfgDotRecurse(std::ostream& os, const ProgramLoaderPtr& programLoader, const Function& func, const VarNodePtr& var) {
  if (var->inputs.size() == 0) return;
  
  for (const auto& in : var->inputs) {
    outputNode(os, programLoader, func, in);
  }

  os << "  "; // indentation

  if (var->inputs.size() == 1) {
    os << getNodeId(var->inputs[0]);
  } else {
    os << "{";
    for (int i = 0; i < var->inputs.size(); i++) {
      os << getNodeId(var->inputs[i]);
      if (i != var->inputs.size()-1)
        os << ", ";
    }
    os << "}";
  }

  os << " -> " << getNodeId(var);

  for (const auto& in : var->inputs) {
    outputDfgDotRecurse(os, programLoader, func, in);
  }
}

void outputDfgDot(std::ostream& os, const ProgramLoaderPtr& programLoader, const Function& func) {
  const DataFlowGraph& dfg = func.dfg;

  os << "strict digraph {\n";

  for (const auto& blockSinks : dfg.sinks) {
    for (const auto& sink : blockSinks) {
      outputNode(os, programLoader, func, sink);
      outputDfgDotRecurse(os, programLoader, func, sink);
    }
  }
  
  os << "\n}\n";
}
}