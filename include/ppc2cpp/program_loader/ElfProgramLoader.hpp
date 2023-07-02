
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
  ElfProgramLoader(const ::google::protobuf::RepeatedPtrField<persistence::ProgramLoaderBinary>& binaryProtos);
  std::optional<ProgramLocation> getReferenceAtLocation(const ProgramLocation& location) override;

private:
  void registerRelocations(std::shared_ptr<ELFIO::elfio> elfPtr, int binaryIdx);
  void registerSymbols(std::shared_ptr<ELFIO::elfio> elfPtr, int binaryIdx);
};
}