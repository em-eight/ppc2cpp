
#pragma once

#include "opcode/ppc.h"

#include "ppc2cpp/model/CpuMemory.hpp"
#include "DataFlowNode.hpp"

namespace ppc2cpp {
/*const uint64_t supported_register_types = 
  PPC_OPERAND_CR_REG |
  PPC_OPERAND_CR_BIT |
  PPC_OPERAND_FPR |
  PPC_OPERAND_GPR |
  PPC_OPERAND_GPR_0 | // if 0, create constant input
  PPC_OPERAND_SPR |
  PPC_OPERAND_GQR;*/

class FlowContext {
public:
  std::vector<VarNodePtr> gprs[32];
  std::vector<VarNodePtr> cr[32]; // one varnode for each bit
  std::vector<VarNodePtr> lr;
  std::vector<VarNodePtr> ctr;
  std::vector<VarNodePtr> xer;
  std::vector<VarNodePtr> fprs[32];
  std::vector<VarNodePtr> fpscr;
  std::vector<VarNodePtr> gqrs[8];
  std::vector<VarNodePtr> ps1s[32];
  
  std::vector<VarNodePtr>& getSprDefinition(int64_t sprNum) {
    if (sprNum == 1) {
      return xer;
    } else if (sprNum == 8) {
      return lr;
    } else if (sprNum == 9) {
      return ctr;
    } else if (912 <= sprNum && sprNum < 920) {
      return gqrs[sprNum - 912];
    } else {
      throw std::runtime_error("Unknown SPR " + std::to_string(sprNum));
    }
  }
  
  std::vector<VarNodePtr>& getDefinition(const CpuMemoryLocation& cpuMemLoc) {
    switch (cpuMemLoc.memspace) {
      case CpuMemorySpace::MEM_SPACE_GPR:
      return gprs[cpuMemLoc.value];
      case CpuMemorySpace::MEM_SPACE_FPR:
      return fprs[cpuMemLoc.value];
      case CpuMemorySpace::MEM_SPACE_CR_BIT:
      return cr[cpuMemLoc.value];
      case CpuMemorySpace::MEM_SPACE_GQR:
      return gqrs[cpuMemLoc.value];
      case CpuMemorySpace::MEM_SPACE_PS1:
      return ps1s[cpuMemLoc.value];
      case CpuMemorySpace::MEM_SPACE_SPR:
      return getSprDefinition(cpuMemLoc.value);
      default:
      throw std::runtime_error("Unknown CPU register space");
    }
  }
  
  void setDefinition(const CpuMemoryLocation& cpuMemLoc, VarNodePtr var) {
    std::vector<VarNodePtr>& def = getDefinition(cpuMemLoc);
    def.clear();
    def.push_back(var);
  }
};
}
