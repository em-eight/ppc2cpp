
#pragma once

#include "Symbol.hpp"
#include "ppc2cpp/control_flow/ControlFlowGraph.hpp"

namespace ppc2cpp {
class Function : public Symbol {
public:
  ControlFlowGraph cfg;

  Function(ProgramLocation location, std::string name) : Symbol(location, name, FUNCTION) {}
  Function(ProgramLocation location, uint32_t size, std::string name) : Symbol(location, size, name, FUNCTION) {}
  // number of instructions in function
  uint32_t length() const { return size/4; }
};
}