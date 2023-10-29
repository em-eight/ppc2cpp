
#pragma once

#include <vector>
#include <unordered_map>
#include <optional>

#include "Relocation.hpp"

namespace ppc2cpp {
class RelocationTable {
private:
  std::vector<Relocation> relocs;
  // index by relocation location
  std::unordered_map<ProgramLocation, uint32_t, ProgramLocation::HashFunction> _source_index;

public:
  Relocation& operator[](uint32_t& idx) { return relocs[idx]; }
  const Relocation& operator[](uint32_t& idx) const { return relocs[idx]; }
  // invalidates indexes!
  void push_back(const Relocation& sym) { relocs.push_back(sym); }
  // prepares _source_index
  void constructSourceIndex();
  std::optional<Relocation> lookupBySource(const ProgramLocation& loc) const;
  std::optional<Relocation> lookupByInsnLoc(const ProgramLocation& loc) const;
  void toProto(persistence::RelocTable* reloctabProto) const;
  static RelocationTable fromProto(const persistence::RelocTable* reloctabProto);
};
}