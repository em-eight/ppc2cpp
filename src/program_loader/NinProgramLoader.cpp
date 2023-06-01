
#include <fstream>

#include "ppc2cpp/common/FileTypeException.hpp"
#include "ppc2cpp/program_loader/NinProgramLoader.hpp"
#include "ppc2cpp/program_loader/Dol.hpp"
#include "ppc2cpp/program_loader/Rel.hpp"

#include "ninutils/dol.hpp"

namespace ppc2cpp {
NinProgramLoader::NinProgramLoader(const ::google::protobuf::RepeatedPtrField<persistence::ProgramLoaderBinary>& binaryProtos) {
  for (const auto& binaryProto : binaryProtos) {
    std::string filename = binaryProto.filename();
    std::string data = binaryProto.data();
    std::string file_extension = filename.substr(filename.find_last_of("."));

    if (file_extension == ".dol") {
      std::shared_ptr<ninutils::Dol> _dolptr = std::make_shared<ninutils::Dol>((uint8_t*) data.c_str(), data.size());
      this->binaries.emplace_back(std::make_shared<Dol>(filename, _dolptr));
    } else if (file_extension == ".rel") {
      std::shared_ptr<ninutils::Rel> _relptr = std::make_shared<ninutils::Rel>((uint8_t*) data.c_str(), data.size());
      this->binaries.emplace_back(std::make_shared<Rel>(filename, _relptr));
    } else {
      throw FileTypeException("Unknown extension " + file_extension);
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
}