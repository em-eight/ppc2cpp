
#pragma once

#include "ppc2cpp/program_loader/ProgramLoader.hpp"
#include "ppc2cpp/model/Function.hpp"

namespace ppc2cpp {

class DataFlowAnalysis {
private:
  ProgramLoaderPtr programLoader;

public:
  DataFlowAnalysis(ProgramLoaderPtr programLoader) : programLoader(programLoader) {}
  void functionDFA(Function& func);
  void functionDFA(Function& func, uint32_t blockIdx);
};
}