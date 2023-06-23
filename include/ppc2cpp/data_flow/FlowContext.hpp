
#pragma once

#include <unordered_set>
#include <algorithm>
#include <iterator>

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

using Definition = std::unordered_set<VarNodePtr>;

// used by data flow analysis to determinate if a block's output FlowContext has changed
class FlowContextSizes {
public:
  int gprs[32];
  int cr[32];
  int lr;
  int ctr;
  int xer;
  int fprs[32];
  int fpscr;
  int gqrs[8];
  int ps1s[32];

  bool operator==(const FlowContextSizes& other) const = default;
  /* {
    if (gprs != other.gprs) return false;
    (fprs != other.fprs) return false;
    (cr != other.cr) return false;
    if (gqrs != other.gqrs) return false;
    if (ps1s != other.ps1s) return false;
    if (lr != other.lr) return false;
    if (ctr != other.ctr) return false;
    if (xer != other.xer) return false;
    if (fpscr != other.fpscr) return false;
    return true;
  }*/
};

class FlowContext {
public:
  Definition gprs[32];
  Definition cr[32]; // one varnode for each bit
  Definition lr;
  Definition ctr;
  Definition xer;
  Definition fprs[32];
  Definition fpscr;
  Definition gqrs[8];
  Definition ps1s[32];

  void contextUnion(const FlowContext& other) {
    for (int i = 0; i < 32; i++) {
      gprs[i].insert(other.gprs[i].begin(), other.gprs[i].end());
    }
    for (int i = 0; i < 32; i++) {
      fprs[i].insert(other.fprs[i].begin(), other.fprs[i].end());
    }
    for (int i = 0; i < 32; i++) {
      cr[i].insert(other.cr[i].begin(), other.cr[i].end());
    }
    for (int i = 0; i < 32; i++) {
      ps1s[i].insert(other.ps1s[i].begin(), other.ps1s[i].end());
    }
    for (int i = 0; i < 8; i++) {
      gqrs[i].insert(other.gqrs[i].begin(), other.gqrs[i].end());
    }
    lr.insert(other.lr.begin(), other.lr.end());
    ctr.insert(other.ctr.begin(), other.ctr.end());
    xer.insert(other.xer.begin(), other.xer.end());
    fpscr.insert(other.fpscr.begin(), other.fpscr.end());
  }

  FlowContextSizes getSizes() const {
    FlowContextSizes ret;
    std::transform(std::begin(this->gprs), std::end(this->gprs), std::begin(ret.gprs), [](const Definition& def){ return def.size(); });
    std::transform(std::begin(this->fprs), std::end(this->fprs), std::begin(ret.fprs), [](const Definition& def){ return def.size(); });
    std::transform(std::begin(this->cr), std::end(this->cr), std::begin(ret.cr), [](const Definition& def){ return def.size(); });
    std::transform(std::begin(this->gqrs), std::end(this->gqrs), std::begin(ret.gqrs), [](const Definition& def){ return def.size(); });
    std::transform(std::begin(this->ps1s), std::end(this->ps1s), std::begin(ret.ps1s), [](const Definition& def){ return def.size(); });
    ret.lr = this->lr.size();
    ret.ctr = this->ctr.size();
    ret.xer = this->xer.size();
    ret.fpscr = this->fpscr.size();
    return ret;
  }
  
  Definition& getSprDefinition(int64_t sprNum) {
    if (sprNum == SPR_VALUE_XER) {
      return xer;
    } else if (sprNum == SPR_VALUE_LR) {
      return lr;
    } else if (sprNum == SPR_VALUE_CTR) {
      return ctr;
    } else if (912 <= sprNum && sprNum < 920) {
      return gqrs[sprNum - 912];
    } else {
      throw std::runtime_error("Unknown SPR " + std::to_string(sprNum));
    }
  }
  
  Definition& getDefinition(const CpuMemoryLocation& cpuMemLoc) {
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
    Definition& def = getDefinition(cpuMemLoc);
    def.clear();
    def.insert(var);
  }
  
  void killDefinition(const CpuMemoryLocation& cpuMemLoc) {
    Definition& def = getDefinition(cpuMemLoc);
    def.clear();
  }
};
}
