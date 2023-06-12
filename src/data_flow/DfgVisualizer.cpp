

#include "ppcdisasm/ppc-dis.hpp"

#include "ppc2cpp/data_flow/DfgVisualizer.hpp"
#include "ppc2cpp/common/endian.h"
#include "ppc2cpp/data_flow/DfgVisualizer.hpp"

using namespace ppcdisasm;

namespace ppc2cpp {
std::string getDotLabel(const ProgramLoaderPtr& programLoader, const Function& func, const VarnodePtr& var) {
  if (OperandnodePtr operandVar = dynamic_pointer_cast<Operandnode>(var)) {
    uint32_t* funcPtr = (uint32_t*) programLoader->getBufferAtLocation(func.location).value();
    const uint32_t insn = be32(*(funcPtr + operandVar->index));
    const struct powerpc_opcode* opcode = ppcdisasm::lookup_powerpc(insn, ppcdisasm::ppc_750cl_dialect);

    if (ResultnodePtr regIn = dynamic_pointer_cast<Resultnode>(var)) {
      return opcode->name;
    } else {
      const struct powerpc_operand* operand = powerpc_operands + operandVar->opindex;
      int64_t value = ppcdisasm::operand_value_powerpc(operand, insn, ppcdisasm::ppc_750cl_dialect);

      std::string regPref = "";
      if ((operand->flags & PPC_OPERAND_GPR) != 0 || ((operand->flags & PPC_OPERAND_GPR_0) != 0 && value != 0))
        regPref += "r";
      else if ((operand->flags & PPC_OPERAND_FPR) != 0)
        regPref += "f";
      return regPref + std::to_string(value);
    }
  } else if (SinknodePtr sinkVar = dynamic_pointer_cast<Sinknode>(var)) {
    uint32_t* funcPtr = (uint32_t*) programLoader->getBufferAtLocation(func.location).value();
    const uint32_t insn = be32(*(funcPtr + sinkVar->index));
    const struct powerpc_opcode* opcode = ppcdisasm::lookup_powerpc(insn, ppcdisasm::ppc_750cl_dialect);
    return opcode->name;

  } else throw std::runtime_error("Cannot create label for Varnode class " + std::string(typeid(var).name())); // TODO: support!
}

void outputNode(std::ostream& os, const ProgramLoaderPtr& programLoader, const Function& func, const VarnodePtr& var) {
  os << "  "; // indentation
  os << var->label << "[label=" + getDotLabel(programLoader, func, var) + "]" << ";\n";
}

void outputDfgDotRecurse(std::ostream& os, const ProgramLoaderPtr& programLoader, const Function& func, const VarnodePtr& var) {
  if (var->inputs.size() == 0) return;
  
  for (const auto& in : var->inputs) {
    outputNode(os, programLoader, func, in);
  }

  os << "  "; // indentation

  if (var->inputs.size() == 1) {
    os << var->inputs[0]->label;
  } else {
    os << "{";
    for (int i = 0; i < var->inputs.size(); i++) {
      os << var->inputs[i]->label;
      if (i != var->inputs.size()-1)
        os << ", ";
    }
    os << "}";
  }

  os << " -> " << var->label;

  for (const auto& in : var->inputs) {
    outputDfgDotRecurse(os, programLoader, func, in);
  }
}

void outputDfgDot(std::ostream& os, const ProgramLoaderPtr& programLoader, const Function& func) {
  const DataFlowGraph& dfg = func.dfg;

  os << "strict digraph {\n";

  for (const auto& sink : dfg.sinks) {
    outputNode(os, programLoader, func, sink);
    outputDfgDotRecurse(os, programLoader, func, sink);
  }
  
  os << "\n}\n";
}
}