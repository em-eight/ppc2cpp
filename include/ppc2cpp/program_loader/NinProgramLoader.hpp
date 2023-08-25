
#pragma once

#include <filesystem>

#include "ProgramLoader.hpp"

namespace ppc2cpp {
/**
 * @brief Program loader for nintendo programs (DOL, REL, RSO etc)
*/
class NinProgramLoader : public ProgramLoader {
public:
  NinProgramLoader(const std::vector<std::filesystem::path>& files);
  std::optional<ProgramLocation> getReferenceAtLocation(const ProgramLocation& location) override;
  int32_t moduleId2binaryIdx(int32_t module_id);
  void registerRel24Relocs(int binary_idx);

  static bool isRvlProgram(const std::vector<std::filesystem::path>& files);
};
}