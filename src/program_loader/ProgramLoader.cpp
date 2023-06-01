
#include "ppc2cpp/program_loader/ProgramLoader.hpp"
#include "ppc2cpp/program_loader/NinProgramLoader.hpp"
#include "ppc2cpp/common/ProjectPersistenceException.hpp"

namespace ppc2cpp {
std::optional<ProgramLocation> ProgramLoader::getLocation(const std::string& binaryName, const std::string& sectionName, uint32_t offset) {
  for (int i = 0; i < binaries.size(); i++) {
    if (binaries[i]->name == binaryName) {
      for (int j = 0; j < binaries[i]->sections.size(); j++) {
        if (binaries[i]->sections[j]->name == sectionName) {
          if (offset < binaries[i]->sections[j]->length) return ProgramLocation(i, j, offset);
        }
      }
    }
  }
  return std::nullopt;
}

std::optional<uint8_t*> ProgramLoader::getBufferAtLocation(const ProgramLocation& location) {
  return binaries[location.binary_idx]->getBufferAtLocation(location);
}
  
std::optional<ProgramLocation> ProgramLoader::resolveVMA(uint32_t vma) {
  for (int i = 0; i < binaries.size(); i++) {
    std::optional<BinaryLocation> maybeLocation = binaries[i]->resolveVMA(vma);
    if (maybeLocation) return ProgramLocation(i, maybeLocation.value());
  }
  return std::nullopt;
}

std::optional<ProgramLocation> ProgramLoader::resolveTarget(const ProgramLocation& pc, int32_t value, bool isAbsolute) {
  // 1. If a reference (reloc) exists at that address, the target is the target of the reference
  if (auto reference = this->getReferenceAtLocation(pc)) {
    return reference;
  }

  if (isAbsolute) {
    // 2. Lookup absolute VMA address
    if (auto vmaLoc = this->resolveVMA(value)) {
      return vmaLoc;
    } else {
      return std::nullopt;
    }
  } else {
    // 3. Lookup location relative to pc
    const auto& section = binaries[pc.binary_idx]->sections[pc.section_idx];
    int32_t targetOff = pc.section_offset + value;
    if (targetOff < 0 || section->length <= targetOff) return std::nullopt;
    else return pc + value;
  }

}

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
    programLoader = std::make_shared<NinProgramLoader>(NinProgramLoader(loaderProto->binaries()));
  } else {
    throw new ProjectPersistenceException("Unknown program loader type " + loaderType);
  }

  programLoader->loaderType = loaderType;
  return programLoader;
}

}