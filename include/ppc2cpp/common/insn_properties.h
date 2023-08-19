
#pragma once

#include <cstdint>

#define OP(x) ((((uint64_t)(x)) >> 26) & 0x3f)

/* X form extended op  */
#define XOP(x) ((((uint64_t)(x)) >> 1) & 0x3ff)

namespace ppc2cpp {
inline bool isBranch(uint32_t insn) {
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

inline bool isBranchWithLink(uint32_t insn) {
  return isLink(insn) && isBranch(insn);
}

inline bool isBranchUncoditional(uint32_t insn) {
  uint32_t op = OP(insn);
  return  op == 18;
}

// Returns the LI field of an I form instruction
inline int32_t branchValue(uint32_t insn) {
  int32_t addr_val = (insn >> 2) & 0xffffff;
  return addr_val;
}

inline bool isBranchConditional(uint32_t insn) {
  uint32_t op = OP(insn);
  return  op == 16;
}

// Returns the BD field of a B form instruction
inline int32_t branchConditionalValue(uint32_t insn) {
  int32_t addr_val = (insn >> 2) & 0x3fff;
  return addr_val;
}

inline bool isBranchToLR(uint32_t insn) {
  uint32_t op = OP(insn);
  uint32_t xop = XOP(insn);
  return  op == 19 && xop == 16;
}

inline bool isBranchToCTR(uint32_t insn) {
  uint32_t op = OP(insn);
  uint32_t xop = XOP(insn);
  return  op == 19 && xop == 528;
}

// BO field of B form instructions
inline uint8_t getBO(uint32_t insn) {
  return (insn >> 21) & 0x1f;
}

inline bool isBranchConditionUnconditional(uint32_t insn) {
  uint8_t bo = getBO(insn);
  return ((bo >> 2) & 0x5) == 0x5;
}

inline bool isLis(uint32_t insn) {
  uint32_t op = OP(insn);
  return  op == 15;
}
}