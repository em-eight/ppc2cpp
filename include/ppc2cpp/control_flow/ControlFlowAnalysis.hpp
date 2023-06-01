
#pragma once

#include "ppc2cpp/program_loader/ProgramLoader.hpp"
#include "ppc2cpp/model/Function.hpp"

namespace ppc2cpp {

class ControlFlowAnalysis {
private:
  ProgramLoaderPtr programLoader;

public:
  ControlFlowAnalysis(ProgramLoaderPtr programLoader) : programLoader(programLoader) {}
  void functionCFA(Function& func);
  void functionCFA(Function& func, uint32_t blockIdx);
  void handleBranch(uint32_t insn, uint32_t blockIdx, Function& func, uint32_t pc, int32_t branchVal);
};
}