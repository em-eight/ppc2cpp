
#include "ppc2cpp/model/SymbolTable.hpp"

namespace ppc2cpp {
void SymbolTable::constructNameIndex() {
  this->_name_index.clear();

  for (int i = 0; i < this->symbols.size(); i++) {
    this->_name_index.insert({this->symbols[i].name, i});
  }
}

std::optional<Symbol> SymbolTable::lookupByName(const std::string& name) const {
  auto val = this->_name_index.find(name);
  if (val != this->_name_index.end()) return this->symbols[val->second];
  else return std::nullopt;
}
}