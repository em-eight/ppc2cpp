
#pragma once

#include "ppc2cpp/model/BinaryLocation.hpp"

namespace ppc2cpp {
// A specification of a location inside a program.
class ProgramLocation : public BinaryLocation {
public:
  // The index of the program loader binary the location refers to
  int32_t binary_idx;

  ProgramLocation() = default;
  ProgramLocation(int32_t binary_idx, int32_t section_idx, uint32_t section_offset) : binary_idx(binary_idx), BinaryLocation(section_idx, section_offset) {}
  ProgramLocation(int32_t binary_idx, BinaryLocation bLoc) : binary_idx(binary_idx), BinaryLocation(bLoc) {}
  ProgramLocation& operator+=(const int32_t& rhs) {
    this->section_offset += rhs;
    return *this;
  }

  friend ProgramLocation operator+(ProgramLocation lhs, const int32_t& rhs) {
    lhs += rhs;
    return lhs;
  }
};
}