
#pragma once

#include "ppcdisasm/ppc-dis.hpp"
#include "opcode/ppc.h"

#include "ppc2cpp/model/CpuMemory.hpp"
#include "PpcOperandCats.h"

namespace ppc2cpp {
const uint64_t supported_register_types = 
  PPC_OPERAND_CR_REG |
  PPC_OPERAND_CR_BIT |
  PPC_OPERAND_FPR |
  PPC_OPERAND_GPR |
  PPC_OPERAND_GPR_0 | // if 0, create constant input
  PPC_OPERAND_SPR |
  PPC_OPERAND_GQR;

extern const std::unordered_set<int32_t> fixedPointInsn;
extern const std::unordered_set<int32_t> floatInsn;
extern const std::unordered_set<int32_t> loadInsn;
extern const std::unordered_set<int32_t> storeInsn;
extern const std::unordered_set<int32_t> branchInsn;
extern std::unordered_set<int32_t> nonStoreInsn;
extern std::unordered_set<int32_t> supportedInsn;

class InstructionUD {
public:
  std::vector<CpuMemoryLocation> inputs;
  std::vector<CpuMemoryLocation> outputs;
  std::vector<int64_t> imms; // Also inputs basically
};

void initInstructionUD();
InstructionUD instructionUD(uint32_t insn, const struct powerpc_opcode* opcode, ppc_cpu_t dialect);
}