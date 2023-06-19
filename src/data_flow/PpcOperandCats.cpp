
#include "ppcdisasm/ppc-operands.h"
#include "opcode/ppc.h"

#include "ppc2cpp/data_flow/PpcOperandCats.h"


namespace ppc2cpp {

bool isImmediate(uint64_t operandFlags, int64_t operandValue) {
  if ((operandFlags & PPC_OPERAND_GPR_0) != 0 && operandValue == 0) return true;

  uint64_t supportedConstantFlags = PPC_OPERAND_SIGNED | PPC_OPERAND_ABSOLUTE | PPC_OPERAND_PARENS | PPC_OPERAND_RELATIVE | PPC_OPERAND_RELATIVE | PPC_OPERAND_PLUS1 |
    PPC_OPERAND_NEGATIVE | PPC_OPERAND_SIGNOPT | PPC_OPERAND_OPTIONAL | PPC_OPERAND_NEXT;

  // has no flags enabled besides the supported
  if ((operandFlags & ~supportedConstantFlags) == 0) return true;
  return false;
}

bool isIgnore(uint64_t operandFlags) {
  if ((operandFlags & PPC_OPERAND_OPTIONAL32) != 0) return true;
  return false;
}
}