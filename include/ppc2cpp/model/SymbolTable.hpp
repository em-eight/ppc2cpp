
#pragma once

#include <vector>
#include <unordered_map>
#include <optional>

#include "Symbol.hpp"

namespace ppc2cpp {
class SymbolTable {
private:
  std::vector<Symbol> symbols;
  // index by symbol name
  std::unordered_map<std::string, uint32_t> _name_index;

public:
  Symbol& operator[](uint32_t& idx) { return symbols[idx]; }
  const Symbol& operator[](uint32_t& idx) const { return symbols[idx]; }
  void push_back(const Symbol& sym) { symbols.push_back(sym); }
  // prepares _name_index
  void constructNameIndex();
  std::optional<Symbol> lookupByName(const std::string& name) const;
};
}