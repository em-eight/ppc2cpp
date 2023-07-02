
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

  Relocation(ProgramLocation source, ProgramLocation destination, uint8_t type) : source(source), destination(destination), type(type) {}
};
}