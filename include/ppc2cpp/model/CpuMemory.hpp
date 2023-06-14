
#pragma once

#include <cstdint>
#include <string>

/**
 * Each storage element in FlowContext is identified by a MemorySpace and a value
*/
namespace ppc2cpp {
enum class CpuMemorySpace {
  MEM_SPACE_GPR,
  MEM_SPACE_FPR,
  MEM_SPACE_CR_BIT,
  MEM_SPACE_GQR,
  // The ps1 in PS instructions. ps0 is FPR
  MEM_SPACE_PS1,
  MEM_SPACE_SPR,
};

inline std::string cpuMemorySpace2prefix(const CpuMemorySpace& memSpace) {
  switch (memSpace) {
  case CpuMemorySpace::MEM_SPACE_GPR:
    return "r";
    break;
  case CpuMemorySpace::MEM_SPACE_FPR:
    return "f";
    break;
  case CpuMemorySpace::MEM_SPACE_CR_BIT:
    return "cr";
    break;
  case CpuMemorySpace::MEM_SPACE_GQR:
    return "gqr";
    break;
  case CpuMemorySpace::MEM_SPACE_PS1:
    return "ps1";
    break;
  case CpuMemorySpace::MEM_SPACE_SPR:
    return "spr";
    break;
  default:
    throw std::runtime_error("Unknown prefix for given memory space");
  }

}

class CpuMemoryLocation {
public:
  CpuMemorySpace memspace;
  int64_t value;
};
}