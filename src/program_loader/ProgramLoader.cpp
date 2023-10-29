
#include <fmt/format.h>

#include "ppc2cpp/common/ProjectPersistenceException.hpp"
#include "ppc2cpp/program_loader/NinProgramLoader.hpp"
#include "ppc2cpp/program_loader/ElfProgramLoader.hpp"
#include "ppc2cpp/program_loader/ProgramLoader.hpp"

#include "symbols.pb.h"
#include "relocs.pb.h"

namespace ppc2cpp {
ProgramLoaderPtr ProgramLoader::loadProgram(int32_t loaderProtoVersion, persistence::ProgramLoaderType loaderType,
    const std::vector<std::filesystem::path>& binary_paths) {
  ProgramLoaderPtr programLoader;
  if (loaderType == persistence::LOADER_RVL) {
    programLoader = std::make_shared<NinProgramLoader>(NinProgramLoader(binary_paths));
  } else if (loaderType == persistence::LOADER_ELF) {
    programLoader = std::make_shared<ElfProgramLoader>(ElfProgramLoader(binary_paths));
  } else {
    throw new ProjectPersistenceException("Unknown program loader type " + std::to_string(static_cast<int>(loaderType)));
  }

  programLoader->loaderType = loaderType;
  return programLoader;
}

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

std::optional<uint32_t> ProgramLoader::getLocationVMA(const ProgramLocation& location) {
  auto maybeAddress = binaries[location.binary_idx]->sections[location.section_idx]->getAddress();
  if (maybeAddress) {
    return maybeAddress.value() + location.section_offset;
  }
  else return std::nullopt;
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
    if (targetOff >= 0 && targetOff < section->length) return pc + value;

    return std::nullopt;
  }
}

void ProgramLoader::toProto(persistence::ProgramLoader* loaderProto) const {
  loaderProto->set_formatversion(PROGRAM_LOADER_PROTO_CURRENT_VERSION);
  loaderProto->set_loadertype(this->loaderType);
  for (const BinaryPtr binary : binaries) {
    persistence::ProgramLoaderBinary* binaryProto = loaderProto->add_binaries();
    binary->toProto(binaryProto);
  }

  this->symtab.toProto(loaderProto->mutable_symtab());
  this->reloctab.toProto(loaderProto->mutable_reloctab());
}

ProgramLoaderPtr ProgramLoader::fromProto(const persistence::ProgramLoader* loaderProto) {
  auto loaderProtoVersion = loaderProto->formatversion();
  auto loaderType = loaderProto->loadertype();

  std::vector<std::filesystem::path> binary_paths;
  std::transform(loaderProto->binaries().begin(), loaderProto->binaries().end(), std::back_inserter(binary_paths), [](const auto& bin) {
    return bin.filepath();
  });
  ProgramLoaderPtr programLoader = ProgramLoader::loadProgram(loaderProtoVersion, loaderType, binary_paths);

  // override symtab and reloctab information if anything is already defined in the protos
  if (loaderProto->symtab().symbols().size() > 0)
    programLoader->symtab = SymbolTable::fromProto(&loaderProto->symtab());
  if (loaderProto->reloctab().relocs().size() > 0)
    programLoader->reloctab = RelocationTable::fromProto(&loaderProto->reloctab());

  return programLoader;
}

std::optional<ProgramLocation> ProgramLoader::resolveByName(const std::string& name) {
  const auto& maybeSymbol = this->symtab.lookupByName(name);
  if (maybeSymbol) return maybeSymbol->location;
  else return std::nullopt;
}

std::string ProgramLoader::locationString(const ProgramLocation& loc) const {
  return "{" + this->binaries[loc.binary_idx]->name + ", " + 
    this->binaries[loc.binary_idx]->sections[loc.section_idx]->name + ", " + fmt::format("0x{:x}", loc.section_offset) + "}";
}
  
int ProgramLoader::findBinaryIdxByName(std::string_view name) const {
  for (int i = 0; i < this->binaries.size(); i++) {
    if (this->binaries[i]->name == name) {
      return i;
    }
  }
  return -1;
}
}