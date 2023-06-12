
#pragma once

#include "DataFlowNode.hpp"
#include "opcode/ppc.h"

namespace ppc2cpp {
/*const uint64_t supported_register_types = 
  PPC_OPERAND_CR_REG |
  PPC_OPERAND_CR_BIT |
  PPC_OPERAND_FPR |
  PPC_OPERAND_GPR |
  PPC_OPERAND_GPR_0 | // if 0, create constant input
  PPC_OPERAND_SPR |
  PPC_OPERAND_GQR;*/

/**
 * Possibly migrate to this class instead of std::vector<VarnodePtr>
*/
class Definition {
public:
  std::vector<VarnodePtr> defs;
};

typedef struct FlowContext {
  std::vector<VarnodePtr> gprs[32];
  std::vector<VarnodePtr> cr[32]; // one varnode for each bit
  std::vector<VarnodePtr> lr;
  std::vector<VarnodePtr> ctr;
  std::vector<VarnodePtr> xer;
  std::vector<VarnodePtr> fprs[32];
  std::vector<VarnodePtr> fpscr;
  std::vector<VarnodePtr> gqrs[8];
  std::vector<VarnodePtr> psfs[32];

  // TODO: refactor code duplication in two functions below
  std::vector<VarnodePtr> getDefinition(const struct powerpc_operand *operand, int64_t value) {
    if ((operand->flags & PPC_OPERAND_GPR) != 0
        || ((operand->flags & PPC_OPERAND_GPR_0) != 0 && value != 0)) {
      return this->gprs[value];
    } else if ((operand->flags & PPC_OPERAND_FPR) != 0) {
      return this->fprs[value];
    } else if ((operand->flags & PPC_OPERAND_GQR) != 0) {
      return this->gqrs[value];
    } else if ((operand->flags & PPC_OPERAND_CR_BIT) != 0) {
      return this->cr[value];
    } else {
      throw std::runtime_error("Cannot determine operand type (value=" + std::to_string(value) + ")");
    }
  }
  
  void setDefinition(const struct powerpc_operand *operand, int64_t value, VarnodePtr var) {
    if ((operand->flags & PPC_OPERAND_GPR) != 0
        || ((operand->flags & PPC_OPERAND_GPR_0) != 0 && value != 0)) {
      this->gprs[value].clear();
      this->gprs[value].push_back(var);
    } else if ((operand->flags & PPC_OPERAND_FPR) != 0) {
      this->fprs[value].clear();
      this->fprs[value].push_back(var);
    } else if ((operand->flags & PPC_OPERAND_GQR) != 0) {
      this->gqrs[value].clear();
      this->gqrs[value].push_back(var);
    } else if ((operand->flags & PPC_OPERAND_CR_BIT) != 0) {
      this->cr[value].clear();
      this->cr[value].push_back(var);
    } else {
      throw std::runtime_error("Cannot determine operand type (value=" + std::to_string(value) + ")");
    }
  }
} FlowContext;
}
