
#include "ppc2cpp/program_loader/Binary.hpp"

namespace ppc2cpp {

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
  binaryProto->set_filename(this->name);
  binaryProto->set_data(this->data, this->size);
}
}