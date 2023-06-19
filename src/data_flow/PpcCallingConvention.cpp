
#include "ppc2cpp/data_flow/PpcCallingConvention.hpp"

namespace ppc2cpp {
const std::unordered_set<CpuMemoryLocation, CpuMemoryLocation::HashFunction> callInputs {
  {CpuMemorySpace::MEM_SPACE_GPR, 3},
  {CpuMemorySpace::MEM_SPACE_GPR, 4},
  {CpuMemorySpace::MEM_SPACE_GPR, 5},
  {CpuMemorySpace::MEM_SPACE_GPR, 6},
  {CpuMemorySpace::MEM_SPACE_GPR, 7},
  {CpuMemorySpace::MEM_SPACE_GPR, 8},
  {CpuMemorySpace::MEM_SPACE_GPR, 9},
  {CpuMemorySpace::MEM_SPACE_GPR, 10},
  
  {CpuMemorySpace::MEM_SPACE_FPR, 1},
  {CpuMemorySpace::MEM_SPACE_FPR, 2},
  {CpuMemorySpace::MEM_SPACE_FPR, 3},
  {CpuMemorySpace::MEM_SPACE_FPR, 4},
  {CpuMemorySpace::MEM_SPACE_FPR, 5},
  {CpuMemorySpace::MEM_SPACE_FPR, 6},
  {CpuMemorySpace::MEM_SPACE_FPR, 7},
  {CpuMemorySpace::MEM_SPACE_FPR, 8},
};

const std::unordered_set<CpuMemoryLocation, CpuMemoryLocation::HashFunction> killedByCall {
  {CpuMemorySpace::MEM_SPACE_GPR, 0},
  {CpuMemorySpace::MEM_SPACE_GPR, 3},
  {CpuMemorySpace::MEM_SPACE_GPR, 4},
  {CpuMemorySpace::MEM_SPACE_GPR, 5},
  {CpuMemorySpace::MEM_SPACE_GPR, 6},
  {CpuMemorySpace::MEM_SPACE_GPR, 7},
  {CpuMemorySpace::MEM_SPACE_GPR, 8},
  {CpuMemorySpace::MEM_SPACE_GPR, 9},
  {CpuMemorySpace::MEM_SPACE_GPR, 10},
  {CpuMemorySpace::MEM_SPACE_GPR, 11},
  {CpuMemorySpace::MEM_SPACE_GPR, 12},

  {CpuMemorySpace::MEM_SPACE_FPR, 0},
  {CpuMemorySpace::MEM_SPACE_FPR, 1},
  {CpuMemorySpace::MEM_SPACE_FPR, 2},
  {CpuMemorySpace::MEM_SPACE_FPR, 3},
  {CpuMemorySpace::MEM_SPACE_FPR, 4},
  {CpuMemorySpace::MEM_SPACE_FPR, 5},
  {CpuMemorySpace::MEM_SPACE_FPR, 6},
  {CpuMemorySpace::MEM_SPACE_FPR, 7},
  {CpuMemorySpace::MEM_SPACE_FPR, 8},
  {CpuMemorySpace::MEM_SPACE_FPR, 9},
  {CpuMemorySpace::MEM_SPACE_FPR, 10},
  {CpuMemorySpace::MEM_SPACE_FPR, 11},
  {CpuMemorySpace::MEM_SPACE_FPR, 12},
  {CpuMemorySpace::MEM_SPACE_FPR, 13},
  
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 0},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 1},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 2},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 3},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 4},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 5},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 6},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 7},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 8},
  // CR2, CR3, CR4 are not volatile (value remains unchanged before and after a call)
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 21},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 22},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 23},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 24},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 25},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 26},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 27},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 28},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 29},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 30},
  {CpuMemorySpace::MEM_SPACE_CR_BIT, 31},
  
  {CpuMemorySpace::MEM_SPACE_SPR, SPR_VALUE_LR},
  {CpuMemorySpace::MEM_SPACE_SPR, SPR_VALUE_XER},
  {CpuMemorySpace::MEM_SPACE_SPR, SPR_VALUE_CTR},
};

const std::unordered_set<CpuMemoryLocation, CpuMemoryLocation::HashFunction> definedByCall {
  {CpuMemorySpace::MEM_SPACE_GPR, 3},
  {CpuMemorySpace::MEM_SPACE_GPR, 4},

  // For  large return values, the return value gets passed through the stack
  // However the stack pointer value itself doesn't change inside function calls, so r1 is not included here

  {CpuMemorySpace::MEM_SPACE_FPR, 1},
};

const std::unordered_set<CpuMemoryLocation, CpuMemoryLocation::HashFunction> globalRegisters {
  {CpuMemorySpace::MEM_SPACE_GPR, 2},
  {CpuMemorySpace::MEM_SPACE_GPR, 13},
  
  {CpuMemorySpace::MEM_SPACE_GQR, 0},
  {CpuMemorySpace::MEM_SPACE_GQR, 1},
  {CpuMemorySpace::MEM_SPACE_GQR, 2},
  {CpuMemorySpace::MEM_SPACE_GQR, 3},
  {CpuMemorySpace::MEM_SPACE_GQR, 4},
  {CpuMemorySpace::MEM_SPACE_GQR, 5},
  {CpuMemorySpace::MEM_SPACE_GQR, 6},
  {CpuMemorySpace::MEM_SPACE_GQR, 7},
};
}