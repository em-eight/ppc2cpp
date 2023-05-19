
#include "ppc2cpp/program_loader/ProgramLoader.hpp"
#include "ppc2cpp/program_loader/NinProgramLoader.hpp"
#include "ppc2cpp/common/ProjectPersistenceException.hpp"

namespace ppc2cpp {

void ProgramLoader::toProto(persistence::ProgramLoader* loaderProto) const {
  loaderProto->set_formatversion(PROGRAM_LOADER_PROTO_CURRENT_VERSION);
  for (const BinaryPtr binary : binaries) {
    persistence::ProgramLoaderBinary* binaryProto = loaderProto->add_binaries();
    binary->toProto(binaryProto);
  }
}

ProgramLoaderPtr ProgramLoader::fromProto(const persistence::ProgramLoader* loaderProto) {
  auto loaderProtoVersion = loaderProto->formatversion();
  auto loaderType = loaderProto->loadertype();

  ProgramLoaderPtr programLoader;
  if (loaderType == persistence::LOADER_RVL) {
    programLoader = std::make_shared<ProgramLoader>(NinProgramLoader(loaderProto->binaries()));
  } else {
    throw new ProjectPersistenceException("Unknown program loader type " + loaderType);
  }

  programLoader->loaderType = loaderType;
  return programLoader;
}

}