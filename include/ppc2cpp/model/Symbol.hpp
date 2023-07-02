
#pragma once

#include <cstdint>
#include <string>

#include "ppc2cpp/model/ProgramLocation.hpp"

namespace ppc2cpp {
enum SymbolType {
  // values match ELF symbol types
  NOTYPE = 0,
  DATA = 1,
  FUNCTION = 2
};

class Symbol {
public:
  std::string name;
  ProgramLocation location;
  uint32_t size;
  SymbolType symbolType;

  // defined location, unknown name
  Symbol(ProgramLocation location, std::string name, SymbolType symType) : location(location), symbolType(symType), name(name) {}
  Symbol(ProgramLocation location, uint32_t size, std::string name, SymbolType symType) : location(location), size(size), symbolType(symType), name(name) {}
  bool contains(const ProgramLocation& loc) const { return location.binary_idx == loc.binary_idx
                                                     && location.section_idx == loc.section_idx
                                                     && location.section_offset <= loc.section_offset
                                                     && loc.section_offset < location.section_offset + size; }
};
}