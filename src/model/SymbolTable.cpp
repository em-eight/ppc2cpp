
#include "ppc2cpp/model/SymbolTable.hpp"

namespace ppc2cpp {
void SymbolTable::constructNameIndex() {
  this->_name_index.clear();

  for (int i = 0; i < this->symbols.size(); i++) {
    if (this->symbols[i].symbolBinding != SymbolBinding::LOCAL) // local symbol can have duplicate names, so don't index them
      this->_name_index.insert({this->symbols[i].name, i});
  }
}

std::optional<Symbol> SymbolTable::lookupByName(const std::string& name) const {
  auto val = this->_name_index.find(name);
  if (val != this->_name_index.end()) return this->symbols[val->second];
  else return std::nullopt;
}

void SymbolTable::constructLocationIndex() {
  this->_loc_index.clear();

  for (int i = 0; i < this->symbols.size(); i++) {
    this->_loc_index.insert({this->symbols[i].location, i});
  }
}

std::optional<Symbol> SymbolTable::lookupByLocation(const ProgramLocation& name) const {
  auto val = this->_loc_index.find(name);
  if (val != this->_loc_index.end()) return this->symbols[val->second];
  else return std::nullopt;
}
}