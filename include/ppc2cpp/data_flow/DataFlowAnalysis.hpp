
#pragma once

#include "ppc2cpp/program_loader/ProgramLoader.hpp"
#include "ppc2cpp/model/Function.hpp"

namespace ppc2cpp {

class DataFlowAnalysis {
private:
  ProgramLoaderPtr programLoader;
  std::vector<int> worklist;

  void initWorklist(const Function& func);
  void initWorklistRecurse(const Function& func, int blockIdx);
  void functionDFAImpl(Function& func);

public:
  DataFlowAnalysis(ProgramLoaderPtr programLoader) : programLoader(programLoader) {}
  void functionDFA(Function& func);
};
}