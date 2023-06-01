
#include "ppc2cpp/program_loader/RelSection.hpp"

namespace ppc2cpp {
RelSection::RelSection(std::shared_ptr<ninutils::Rel> relPtr, ninutils::RelSection relsection) : _relPtr(relPtr) {
  this->name = relsection.name;
  this->offset = relsection.offset;
  this->length = relsection.length;
  this->type = this->length <= 0 ? SECTION_TYPE_NOLOAD : (relsection.exec ? SECTION_TYPE_TEXT : (relsection.isBss() ? SECTION_TYPE_BSS : SECTION_TYPE_DATA));
}

std::optional<uint8_t*> RelSection::getBufferAtOffset(uint32_t offset) const {
  if (offset < this->length)
    return _relPtr->file + this->offset + offset;
  return std::nullopt;
}

std::optional<uint8_t*> RelSection::getBufferAtAddress(uint32_t vma) const {
  if (_relPtr->load_addr != 0)
    return getBufferAtOffset(vma - _relPtr->load_addr);
  return std::nullopt;
}
  
std::optional<uint32_t> RelSection::getAddress() const {
  if (_relPtr->load_addr != 0)
    return _relPtr->load_addr + this->offset;
  return std::nullopt;
}
}