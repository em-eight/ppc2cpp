
#pragma once

#include "ppc2cpp/program_loader/ProgramLoader.hpp"
#include "ppc2cpp/model/Function.hpp"

namespace ppc2cpp {
class ProgramComparator {
public:
  ProgramLoaderPtr pLoader1;
  ProgramLoaderPtr pLoader2;

  ProgramComparator(ProgramLoaderPtr pLoader1, ProgramLoaderPtr pLoader2) : pLoader1(pLoader1), pLoader2(pLoader2) {}

  bool compareFunctionFlows(const Function& func1, const Function& func2);

private:
  uint32_t* func1ptr;
  uint32_t* func2ptr;

  bool compareVarNodes(const Function& func1, const Function& func2, const VarNodePtr& var1, const VarNodePtr& var2);
};
}