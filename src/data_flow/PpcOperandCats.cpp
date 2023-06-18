
#include "ppcdisasm/ppc-operands.h"
#include "opcode/ppc.h"

#include "ppc2cpp/data_flow/PpcOperandCats.h"


namespace ppc2cpp {

const std::unordered_set<int32_t> imm_operands = {
  BD,
  BDA,
  BDM,
  BDMA,
  BDP,
  BDPA,
  BFF,
  BO,
  BOM,
  BOP,
  BH,
  // not finished! Abandoned for isImmediate

  D,
};

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

const std::unordered_set<int32_t> input_operands = {
  // CR operands
  BI,
  BTAB,
  BB,
  BAB,
  BFA,
  CR,

  // FPR operands
  FRA,
  FRB,
  FRC,
  FRS,
  
  // GPR operands
  RA,
  RA0,
  RAQ,
  PRA0,
  PRAQ,
  //RAM // lmw
  RB,
  RSB,
  RBX,
  RS,
  // load with update
  RAL,
  RAS,

  // PS operands
  PSQ,
  PSQM,
};

const std::unordered_set<int32_t> output_operands = {
  // CR operands
  BTAB,
  BT,
  BF,
  OBF,
  BTF,
  
  // FPR operands
  FRT,
  
  // GPR operands
  RT,
  RAL,
  RAS,
};
}