
#pragma once

#include <vector>
#include <filesystem>

#include "Binary.hpp"

namespace ppc2cpp {
/**
 * @brief An interface to access a program's sections, code, data, symbols and relocations.
 * Inputs can be binary, map files, or any other kind of file that provides information about a loaded program.
 * Note that this object only provides information that is obvious from the files themselves.
 * Further information is mined in the decompiler's analysis passes
*/
class ProgramLoader {
public:
  std::vector<BinaryPtr> binaries;
  //virtual std::optional<uint8_t*> getBufferAtOffset(uint32_t moduleId, uint32_t sectionIdx, uint32_t offset) = 0;
  //virtual std::optional<uint8_t*> getBufferAtAddress(uint32_t vma) = 0;
};
}