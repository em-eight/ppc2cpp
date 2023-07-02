
#include "ppc2cpp/program_loader/ElfSection.hpp"

using namespace ELFIO;

namespace ppc2cpp {
ElfSection::ElfSection(const ELFIO::section* elfsec) {
  this->name = elfsec->get_name();
  this->offset = elfsec->get_offset();
  this->length = elfsec->get_size();
  this->address = elfsec->get_address();
  this->_data = (uint8_t*) elfsec->get_data();
  if ((elfsec->get_flags() & SHF_ALLOC) != 0) {
    this->type = SECTION_TYPE_NOLOAD;
  } else {
    this->type = elfsec->get_type() == SHT_NOBITS ? SECTION_TYPE_BSS : (((elfsec->get_flags() & SHF_EXECINSTR) != 0) ? SECTION_TYPE_TEXT : SECTION_TYPE_DATA);
  }
}

const ELFIO::section* find_section_by_offset(std::shared_ptr<ELFIO::elfio> elfPtr, uint32_t offset) {
  for (int i = 0; i < elfPtr->sections.size(); i++) {
    if (elfPtr->sections[i]->get_offset() == offset) {
      return elfPtr->sections[i];
    }
  }
  return nullptr;
}

ElfSection::ElfSection(std::shared_ptr<ELFIO::elfio> elfPtr, const ELFIO::segment* elfseg) {
  const ELFIO::section* elfsec = find_section_by_offset(elfPtr, elfseg->get_offset());
  if (elfsec == nullptr) throw std::runtime_error("Failed to find segment " + std::to_string(elfseg->get_index()) + " corresponding section");

  this->name = elfsec->get_name();
  this->offset = elfseg->get_offset();
  this->length = elfseg->get_memory_size();
  this->address = elfseg->get_virtual_address();
  this->_data = (uint8_t*) elfseg->get_data();
  if (elfseg->get_type() != PT_LOAD) {
    this->type = SECTION_TYPE_NOLOAD;
  } else {
    this->type = elfsec->get_type() == SHT_NOBITS ? SECTION_TYPE_BSS : (((elfsec->get_flags() & SHF_EXECINSTR) != 0) ? SECTION_TYPE_TEXT : SECTION_TYPE_DATA);
  }
}

std::optional<uint8_t*> ElfSection::getBufferAtOffset(uint32_t offset) const {
  if (offset < this->length)
    return this->_data + offset;
  return std::nullopt;
}

std::optional<uint8_t*> ElfSection::getBufferAtAddress(uint32_t vma) const {
  return getBufferAtOffset(vma - this->address);
}
  
std::optional<uint32_t> ElfSection::getAddress() const {
  if (this-> address == 0) return std::nullopt;
  else return this->address;
}
}