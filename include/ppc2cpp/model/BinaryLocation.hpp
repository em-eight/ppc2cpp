
#pragma once

#include <cstdint>

namespace ppc2cpp {
// A specification of location inside a binary
class BinaryLocation {
public:
  int32_t section_idx;
  uint32_t section_offset;

  BinaryLocation(int32_t section_idx, uint32_t section_offset) : section_idx(section_idx), section_offset(section_offset) {}
  bool operator==(const BinaryLocation& other) const { return section_idx == other.section_idx && section_offset == other.section_offset; }
  BinaryLocation& operator+=(const int32_t& rhs) {
    this->section_offset += rhs;
    return *this;
  }

  friend BinaryLocation operator+(BinaryLocation lhs, const int32_t& rhs) {
    lhs += rhs;
    return lhs;
  }

  struct HashFunction {
    size_t operator()(const BinaryLocation& loc) const {
      size_t secHash = std::hash<uint8_t>()(loc.section_idx);
      size_t offHash = std::hash<uint32_t>()(loc.section_offset) << 1;
      return secHash ^ offHash;
    }
  };
};
}