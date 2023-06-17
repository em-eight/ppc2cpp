
#pragma once

#include <cstdint>
#include <string>

#define SPR_VALUE_LR 8
#define SPR_VALUE_XER 1
#define SPR_VALUE_CTR 9

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

class CpuMemoryLocation {
public:
  CpuMemorySpace memspace;
  int64_t value;
};

inline std::string sprAlias(int64_t sprNum) {
    if (sprNum == SPR_VALUE_XER) {
      return "xer";
    } else if (sprNum == SPR_VALUE_LR) {
      return "lr";
    } else if (sprNum == SPR_VALUE_CTR) {
      return "ctr";
    } else if (912 <= sprNum && sprNum < 920) {
      return "gqr" + std::to_string(sprNum - 912);
    } else {
      throw std::runtime_error("Unknown SPR " + std::to_string(sprNum));
    }
}

inline std::string cpuMemorySpace2label(const CpuMemoryLocation& memLoc) {
  switch (memLoc.memspace) {
  case CpuMemorySpace::MEM_SPACE_GPR:
    return "r" + std::to_string(memLoc.value);
    break;
  case CpuMemorySpace::MEM_SPACE_FPR:
    return "f" + std::to_string(memLoc.value);
    break;
  case CpuMemorySpace::MEM_SPACE_CR_BIT:
    return "cr" + std::to_string(memLoc.value);
    break;
  case CpuMemorySpace::MEM_SPACE_GQR:
    return "gqr" + std::to_string(memLoc.value);
    break;
  case CpuMemorySpace::MEM_SPACE_PS1:
    return "ps1" + std::to_string(memLoc.value);
    break;
  case CpuMemorySpace::MEM_SPACE_SPR:
    return sprAlias(memLoc.value);
    break;
  default:
    throw std::runtime_error("Unknown prefix for given memory space");
  }
}
}