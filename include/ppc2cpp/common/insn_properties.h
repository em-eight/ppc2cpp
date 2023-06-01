
#pragma once

#include <cstdint>

#define OP(x) ((((uint64_t)(x)) >> 26) & 0x3f)

/* X form extended op  */
#define XOP(x) ((((uint64_t)(x)) >> 1) & 0x3ff)

namespace ppc2cpp {
bool isBranch(uint32_t insn) {
  uint32_t op = OP(insn);
  if (op == 18 || op == 16) {
    return true;
  }
  uint32_t xop = XOP(insn);
  if ((op == 19 && (xop == 528 || xop == 16))) {
    return true;
  }
  return false;
}

// If branch, sets link register before branch
inline bool isLink(uint32_t insn) { return (insn & 1) == 1; }

// for branches to address, whether the address is absolute or not
inline bool isAbsolute(uint32_t insn) { return ((insn >> 1) & 1) == 1; }

bool isBranchWithLink(uint32_t insn) {
  return isLink(insn) && isBranch(insn);
}

bool isBranchUncoditional(uint32_t insn) {
  uint32_t op = OP(insn);
  return  op == 18;
}

// Returns the LI field of an I form instruction
int32_t branchValue(uint32_t insn) {
  int32_t addr_val = (insn >> 2) & 0xffffff;
  return addr_val;
}

bool isBranchConditional(uint32_t insn) {
  uint32_t op = OP(insn);
  return  op == 16;
}

// Returns the BD field of a B form instruction
int32_t branchConditionalValue(uint32_t insn) {
  int32_t addr_val = (insn >> 2) & 0x3fff;
  return addr_val;
}

bool isBranchToLR(uint32_t insn) {
  uint32_t op = OP(insn);
  uint32_t xop = XOP(insn);
  return  op == 19 && xop == 16;
}

bool isBranchToCTR(uint32_t insn) {
  uint32_t op = OP(insn);
  uint32_t xop = XOP(insn);
  return  op == 19 && xop == 528;
}

// BO field of B form instructions
inline uint8_t getBO(uint32_t insn) {
  return (insn >> 21) & 0x1f;
}

bool isBranchConditionUnconditional(uint32_t insn) {
  uint8_t bo = getBO(insn);
  return ((bo >> 2) & 0x5) == 0x5;
}
}