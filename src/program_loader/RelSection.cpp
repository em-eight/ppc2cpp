
#include "ppc2cpp/program_loader/RelSection.hpp"

namespace ppc2cpp {
RelSection::RelSection(std::shared_ptr<ninutils::Rel> relPtr, ninutils::RelSection relsection) : _relPtr(relPtr) {
  this->name = relsection.name;
  this->offset = relsection.offset;
  this->length = relsection.length;
  this->type = this->length <= 0 ? SECTION_TYPE_NOLOAD : (relsection.exec ? SECTION_TYPE_TEXT : (relsection.isBss() ? SECTION_TYPE_BSS : SECTION_TYPE_DATA));
  this->setLoadInfo(_relPtr->load_addr, _relPtr->bss_load_addr);
}

void RelSection::setLoadInfo(uint32_t rel_load_addr, uint32_t bss_load_addr) {
  if (this->type == SECTION_TYPE_BSS) {
    this->load_address = bss_load_addr;
  } else {
    this->load_address = rel_load_addr + this->offset;
  }
}

std::optional<uint8_t*> RelSection::getBufferAtOffset(uint32_t offset) const {
  if (offset < this->length)
    return _relPtr->file + this->offset + offset;
  return std::nullopt;
}

std::optional<uint8_t*> RelSection::getBufferAtAddress(uint32_t vma) const {
  if (this->load_address != 0)
    return getBufferAtOffset(vma - this->load_address);
  return std::nullopt;
}
  
std::optional<uint32_t> RelSection::getAddress() const {
  if (this->load_address != 0)
    return this->load_address;
  return std::nullopt;
}
}