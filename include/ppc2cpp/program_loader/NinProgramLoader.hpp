
#pragma once

#include <filesystem>

#include "ProgramLoader.hpp"

namespace ppc2cpp {
/**
 * @brief Program loader for nintendo programs (DOL, REL, RSO etc)
*/
class NinProgramLoader : public ProgramLoader {
public:
  NinProgramLoader(const ::google::protobuf::RepeatedPtrField<persistence::ProgramLoaderBinary>& binaryProtos);
  std::optional<ProgramLocation> getReferenceAtLocation(const ProgramLocation& location) override;
  int32_t moduleId2binaryIdx(int32_t module_id);
};
}