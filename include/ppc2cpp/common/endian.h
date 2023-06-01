
#pragma once

#include <cstdint> 
#include <bit> 

// very useful
inline uint8_t be8(uint8_t x) { return x; }

inline uint16_t be16(uint16_t x) {
  if constexpr (std::endian::native != std::endian::big) return ((x & 0xff) >> 8) | (x << 8);
  return x;
}

inline uint32_t be32(uint32_t num) {
  if constexpr (std::endian::native != std::endian::big) return ((num & 0xff000000) >> 24) | ((num & 0x00ff0000) >> 8) | ((num & 0x0000ff00) << 8) | (num << 24);
  return num;
}