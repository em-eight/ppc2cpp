
#pragma once

#include <filesystem>

#include <elfio/elfio.hpp>

#include "ProgramLoader.hpp"

namespace ppc2cpp {
/**
 * @brief Program loader for nintendo programs (DOL, REL, RSO etc)
*/
class ElfProgramLoader : public ProgramLoader {
public:
  ElfProgramLoader(const std::vector<std::filesystem::path>& binaryPaths);
  std::optional<ProgramLocation> getReferenceAtLocation(const ProgramLocation& location) override;

  static bool isElfProgram(const std::vector<std::filesystem::path>& files);

private:
  ProgramLocation getElfSymbolLocation(int binaryIdx, ELFIO::Elf_Half section_index, ELFIO::Elf64_Addr value, bool interpretSymValueAsAddr);
  ProgramLocation getElfRelocDest(const ELFIO::symbol_section_accessor& accessor, int sym_idx, int binaryIdx, const ProgramLocation& relocSource,
    bool interpretSymValueAsAddr);
  void registerRelocations(std::shared_ptr<ELFIO::elfio> elfPtr, int binaryIdx);
  void registerSymbols(std::shared_ptr<ELFIO::elfio> elfPtr, int binaryIdx);
};
}