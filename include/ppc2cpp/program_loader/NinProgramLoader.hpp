
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
};
}