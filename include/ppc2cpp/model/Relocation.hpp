
#pragma once

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
};
}