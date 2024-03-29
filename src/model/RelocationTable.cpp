
#include "ppc2cpp/model/RelocationTable.hpp"

namespace ppc2cpp {
void RelocationTable::constructSourceIndex() {
  this->_source_index.clear();

  for (int i = 0; i < this->relocs.size(); i++) {
    this->_source_index.insert({this->relocs[i].source, i});
  }
}

std::optional<Relocation> RelocationTable::lookupBySource(const ProgramLocation& name) const {
  auto val = this->_source_index.find(name);
  if (val != this->_source_index.end()) return this->relocs[val->second];
  else return std::nullopt;
}

std::optional<Relocation> RelocationTable::lookupByInsnLoc(const ProgramLocation& loc) const {
  auto maybeReloc = this->lookupBySource(loc);
  // LO HA and HI relocations are applied to the latter half part of the instruction
  if (!maybeReloc) return this->lookupBySource(loc + 2);
  return maybeReloc;
}

void RelocationTable::toProto(persistence::RelocTable* reloctabProto) const {
  for (const auto& reloc : relocs) {
    reloc.toProto(reloctabProto->add_relocs());
  }
}

RelocationTable RelocationTable::fromProto(const persistence::RelocTable* reloctabProto) {
  RelocationTable reloctab;
  for (const auto& relocProto : reloctabProto->relocs()) {
    reloctab.push_back(Relocation::fromProto(&relocProto));
  }

  reloctab.constructSourceIndex();

  return reloctab;
}
}