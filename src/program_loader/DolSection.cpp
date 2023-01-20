
#include "ppc2cpp/program_loader/DolSection.hpp"

namespace ppc2cpp {
DolSection::DolSection(std::shared_ptr<ninutils::Dol> dolPtr, ninutils::DolSection dolsection) : _dolPtr(dolPtr) {
  this->name = dolsection.name;
  this->offset = dolsection.offset;
  this->length = dolsection.length;
  this->address = dolsection.address;
  this->type = dolsection.isText ? SECTION_TYPE_TEXT : (dolsection.isBss() ? SECTION_TYPE_BSS : SECTION_TYPE_DATA);
}

std::optional<uint8_t*> DolSection::getBufferAtOffset(uint32_t offset) const {
  if (offset < this->length)
    return _dolPtr->file + this->offset + offset;
  return std::nullopt;
}

std::optional<uint8_t*> DolSection::getBufferAtAddress(uint32_t vma) const {
  return getBufferAtOffset(vma - this->address);
}
  
std::optional<uint32_t> DolSection::getAddress() const {
  return this->address;
}
}