
#pragma once

#include "relocs.pb.h"

#include "ProgramLocation.hpp"

namespace ppc2cpp {
class Relocation {
public:
  // Program location that the relocation applies to
  ProgramLocation source;
  // Program location that the relocation references
  ProgramLocation destination;
  // relocation type, as defined by the architecture
  uint8_t type;
  int32_t addend;

  Relocation(ProgramLocation source, ProgramLocation destination, uint8_t type) : source(source), destination(destination), type(type), addend(0) {}
  Relocation(ProgramLocation source, ProgramLocation destination, uint8_t type, int32_t addend) : source(source), destination(destination), type(type), addend(addend) {}
  void toProto(persistence::Relocation* relocProto) const {
    source.toProto(relocProto->mutable_location());
    destination.toProto(relocProto->mutable_target());
    relocProto->set_type(type);
    relocProto->set_addend(addend);
  }
  static Relocation fromProto(const persistence::Relocation* relocProto) {
    return Relocation(ProgramLocation::fromProto(&relocProto->location()), ProgramLocation::fromProto(&relocProto->target()), relocProto->type(), relocProto->addend());
  }
};
}