
#pragma once

#include <unordered_set>

#include "ppc2cpp/model/CpuMemory.hpp"

namespace ppc2cpp {
// aka function arguments, aka memory locations in the calling function that a called function can use
extern const std::unordered_set<CpuMemoryLocation, CpuMemoryLocation::HashFunction> callInputs;
// aka volatile memory locations, aka memory locations in the calling function that are considered not defined after a called function returns
extern const std::unordered_set<CpuMemoryLocation, CpuMemoryLocation::HashFunction> killedByCall;
// aka function return values, aka memory location a called function can affect in the calling function
extern const std::unordered_set<CpuMemoryLocation, CpuMemoryLocation::HashFunction> definedByCall;
// Registers that are considered to maintain a spcific value accross compiled functions. Writing to theses registers is not supported
//const std::unordered_set<CpuMemoryLocation> globalRegisters;
}