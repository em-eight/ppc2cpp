
#include <fstream>
#include <memory>

#include "ppc2cpp/common/FileTypeException.hpp"
#include "ppc2cpp/common/endian.h"
#include "ppc2cpp/common/insn_properties.h"
#include "ppc2cpp/program_loader/NinProgramLoader.hpp"
#include "ppc2cpp/program_loader/Dol.hpp"
#include "ppc2cpp/program_loader/Rel.hpp"

#include "ninutils/dol.hpp"

namespace ppc2cpp {
NinProgramLoader::NinProgramLoader(const std::vector<std::filesystem::path>& binaryPaths) {
  for (const auto& binaryPath : binaryPaths) {
    std::string filename = binaryPath.filename();
    std::ifstream fstream(binaryPath, std::ios::binary | std::ios::in);
    if (!fstream.is_open()) throw std::runtime_error("Input file " + binaryPath.string() + " was not found");
    std::stringstream sstream;
    sstream << fstream.rdbuf();
    std::string data = sstream.str();
    std::string file_extension = filename.substr(filename.find_last_of("."));

    if (file_extension == ".dol") {
      this->binaries.emplace_back(std::make_shared<Dol>(binaryPath));
    } else if (file_extension == ".rel") {
      this->binaries.emplace_back(std::make_shared<Rel>(binaryPath));
    } else {
      throw FileTypeException("Unknown extension " + file_extension);
    }
  }

  for (int i = 0; i < this->binaries.size(); i++) {
    if (dynamic_pointer_cast<Rel>(this->binaries[i])) {
      registerRel24Relocs(i);
    }
  }
}

void NinProgramLoader::registerRel24Relocs(int binary_idx) {
  RelPtr rel = static_pointer_cast<Rel>(this->binaries[binary_idx]);
  for (int i = 0; i < rel->sections.size(); i++) {
    const auto& section = rel->sections[i];
    if (section->type == BinarySection::Type::SECTION_TYPE_TEXT) {
      uint32_t* buf = reinterpret_cast<uint32_t*>(section->getBuffer());
      for (int j = 0; j < section->length/4; j++) {
        uint32_t insn = be32(*(buf + j));
        if (isBranchUncoditional(insn)) {
          ProgramLocation source(binary_idx, i, 4*j);
          int32_t branchVal = branchValue(insn);
          int32_t targetOff = section->offset + 4*j + branchVal;
          auto maybeTargetSection = rel->findSectionByOffset(targetOff);
          if (maybeTargetSection.has_value()) {
            ProgramLocation target(binary_idx, maybeTargetSection.value(), targetOff);
            this->reloctab.push_back(Relocation(source, target, R_PPC_REL24, 0));
          }
        }
      }
    }
  }
}

int32_t NinProgramLoader::moduleId2binaryIdx(int32_t module_id) {
  int32_t binary_idx = -1;
  for (uint32_t i = 0; i < binaries.size(); i++) {
    BinaryPtr binary = binaries[i];
    if (RelPtr relBinary = dynamic_pointer_cast<Rel>(binary)) {
      if (relBinary->getModuleId() == module_id)
        binary_idx = i;
    } else if (DolPtr dolBinary = dynamic_pointer_cast<Dol>(binary)) {
      if (module_id == 0)
        binary_idx = i;
    }
  }
  return binary_idx;
}

std::optional<ProgramLocation> NinProgramLoader::getReferenceAtLocation(const ProgramLocation& location) {
  BinaryPtr binary = binaries[location.binary_idx];
  if (dynamic_pointer_cast<Dol>(binary)) {
    return std::nullopt;
  } else if (RelPtr relBinary = dynamic_pointer_cast<Rel>(binary)) {
    const auto& maybeRelReloc = relBinary->getRelocAtLocation(location);
    if (!maybeRelReloc) return std::nullopt;
    const auto& relReloc = maybeRelReloc.value();

    if (relReloc.src_module_id == 0) { // relocations to DOL are directly VMAs of symbol. TODO: rename src_offset to addend
      return this->resolveVMA(relReloc.src_offset);
    }

    int32_t binary_idx = moduleId2binaryIdx(relReloc.src_module_id);
    if (binary_idx < 0) throw std::runtime_error("Could not find module with id " + relReloc.src_module_id);

    RelPtr targetRel = static_pointer_cast<Rel>(binaries[binary_idx]);

    return ProgramLocation(binary_idx, relReloc.src_section_idx, relReloc.src_offset);
  } else
    throw std::runtime_error("Unsupported RVL binary type at index " + location.binary_idx);
}

bool NinProgramLoader::isRvlProgram(const std::vector<std::filesystem::path>& files) {
  for (const auto& file : files) {
    // sophisticated RVL binary check
    if (file.extension() != ".rel" && file.extension() != ".dol")
      return false;
  }
  return true;
}
}