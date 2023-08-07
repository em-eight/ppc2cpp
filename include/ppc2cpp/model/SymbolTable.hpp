
#pragma once

#include <vector>
#include <unordered_map>
#include <optional>

#include "symbols.pb.h"

#include "Symbol.hpp"

namespace ppc2cpp {
class SymbolTable {
private:
  std::vector<Symbol> symbols;
  // index by symbol name
  std::unordered_map<std::string, uint32_t> _name_index;
  // index by symbol location
  std::unordered_map<ProgramLocation, uint32_t, ProgramLocation::HashFunction> _loc_index;

public:
  int num_symbols() const { return symbols.size(); }
  Symbol& operator[](int& idx) { return symbols[idx]; }
  const Symbol& operator[](int& idx) const { return symbols[idx]; }
  // invalidates indexes!
  void push_back(const Symbol& sym) { symbols.push_back(sym); }
  // prepares _name_index
  void constructNameIndex();
  std::optional<Symbol> lookupByName(const std::string& name) const;
  // prepares _loc_index
  void constructLocationIndex();
  std::optional<Symbol> lookupByLocation(const ProgramLocation& loc) const;

  void toProto(persistence::SymbolTable* symtabProto) const;
  static SymbolTable fromProto(const persistence::SymbolTable* symtabProto);
};
}