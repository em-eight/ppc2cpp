
#pragma once

#include <cstdint>
#include <string>

#include "symbols.pb.h"

#include "ppc2cpp/model/ProgramLocation.hpp"

namespace ppc2cpp {
enum SymbolType {
  // values match ELF symbol types
  NOTYPE = 0,
  DATA = 1,
  FUNCTION = 2
};

enum SymbolBinding {
  // values match ELF symbol binding
  LOCAL = 0,
  GLOBAL = 1,
  WEAK = 2
};

class Symbol {
public:
  std::string name;
  ProgramLocation location;
  uint32_t size;
  SymbolType symbolType;
  SymbolBinding symbolBinding;

  // defined location, unknown name
  Symbol(ProgramLocation location, std::string name, SymbolType symType) : location(location), symbolType(symType), name(name) {}
  Symbol(ProgramLocation location, uint32_t size, std::string name, SymbolType symType) : location(location), size(size), symbolType(symType), name(name) {}
  Symbol(ProgramLocation location, uint32_t size, std::string name, SymbolType symType, SymbolBinding symBinding) : location(location),
    size(size), symbolType(symType), name(name), symbolBinding(symBinding) {}
  bool contains(const ProgramLocation& loc) const { return location.binary_idx == loc.binary_idx
                                                     && location.section_idx == loc.section_idx
                                                     && location.section_offset <= loc.section_offset
                                                     && loc.section_offset < location.section_offset + size; }
  void toProto(persistence::Symbol* symProto) const {
    symProto->set_name(name);
    location.toProto(symProto->mutable_location());
    symProto->set_size(size);
    symProto->set_st_type(static_cast<uint32_t>(symbolType));
    symProto->set_st_bind(static_cast<uint32_t>(symbolBinding));
  }

  static Symbol fromProto(const persistence::Symbol* symProto) {
    return Symbol(ProgramLocation::fromProto(&symProto->location()), symProto->size(), symProto->name(),
      static_cast<SymbolType>(symProto->st_type()), static_cast<SymbolBinding>(symProto->st_bind()));
  }
};
}