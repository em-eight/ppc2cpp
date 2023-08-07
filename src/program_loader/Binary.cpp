
#include <fstream>
#include <sstream>

#include "ppc2cpp/program_loader/Binary.hpp"

namespace ppc2cpp {
Binary::Binary(std::filesystem::path binaryPath) {
  this->filepath = binaryPath;
  this->name = binaryPath.filename();

  std::ifstream fstream(binaryPath, std::ios::binary | std::ios::in);
  if (!fstream.is_open()) throw std::runtime_error("Input file " + binaryPath.string() + " was not found");
  std::stringstream sstream;
  sstream << fstream.rdbuf();
  this->data = sstream.str(); // copy binary data to buffer
}

std::optional<BinaryLocation> Binary::resolveVMA(uint32_t vma) {
  for (int i = 0; i < sections.size(); i++) {
    const auto& addr = sections[i]->getAddress();
    if (addr.has_value()) {
      if (addr.value() <= vma && vma < addr.value() + sections[i]->length) return BinaryLocation(i, vma - addr.value());
    }
  }
  return std::nullopt;
}

void Binary::toProto(persistence::ProgramLoaderBinary* binaryProto) const {
  binaryProto->set_filepath(this->filepath);
  binaryProto->set_data(this->data);
}
}