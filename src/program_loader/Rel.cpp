
#include "ppc2cpp/program_loader/Rel.hpp"
#include "ppc2cpp/program_loader/RelSection.hpp"

namespace ppc2cpp {
void Rel::genSectionName(uint8_t sec) { 
  // generate sensible default name
  if (sections[sec]->type == BinarySection::SECTION_TYPE_NOLOAD) {
    sections[sec]->name = "";
    return;
  }

  uint8_t similarSectionTypeCount = 0;
  for (uint8_t i = 0; i < sections.size(); i++) {
    if (i == sec) continue;

    if (sections[i]->type == sections[sec]->type) {
        similarSectionTypeCount++;
    }
  }
  std::string sectionBaseName;
  switch (sections[sec]->type) {
  case BinarySection::SECTION_TYPE_TEXT:
    sectionBaseName = ".text";
    break;
  case BinarySection::SECTION_TYPE_BSS:
    sectionBaseName = ".bss";
    break;
  case BinarySection::SECTION_TYPE_DATA:
    sectionBaseName = ".data";
    break;
  
  default:
    sectionBaseName = "";
    break;
  }
  if (similarSectionTypeCount == 0) {
      sections[sec]->name = sectionBaseName;
  } else {
      sections[sec]->name = sectionBaseName + std::to_string(similarSectionTypeCount+1);
  }
}

Rel::Rel(const std::filesystem::path& filepath) : Binary(filepath) {
  this->_relPtr = std::make_shared<ninutils::Rel>((uint8_t*)this->data.c_str(), this->data.size());

  this->sections.reserve(_relPtr->secs_raw.size());
  for (const auto& ninrelsec : _relPtr->secs_raw) {
    this->sections.push_back(std::make_shared<RelSection>(RelSection(_relPtr, ninrelsec)));
    this->genSectionName(this->sections.size()-1);
  }

  for (const auto& relReloc : _relPtr->rels) {
    BinaryLocation relocLocation = {relReloc.section_idx, relReloc.offset};
    relocLUT.insert({relocLocation, relReloc});
  }
}

int32_t Rel::getModuleId() {
  return _relPtr->hdr.id;
}

std::optional<ninutils::RelReloc> Rel::getRelocAtLocation(const BinaryLocation& location) {
  if (!relocLUT.contains(location)) return std::nullopt;

  const ninutils::RelReloc& relReloc = relocLUT.at(location);
  return relReloc;
}

void Rel::setLoadInfo(uint32_t rel_load_addr, uint32_t bss_load_addr) {
  for (auto sec : this->sections) {
    RelSectionPtr relsec = static_pointer_cast<RelSection>(sec);
    relsec->setLoadInfo(rel_load_addr, bss_load_addr);
  }
}
}