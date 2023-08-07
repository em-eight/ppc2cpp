
#include "ppc2cpp/program_loader/Dol.hpp"
#include "ppc2cpp/program_loader/DolSection.hpp"

namespace ppc2cpp {
Dol::Dol(const std::filesystem::path& filepath) : Binary(filepath) {
  this->_dolPtr = std::make_shared<ninutils::Dol>((uint8_t*)this->data.c_str(), this->data.size());
  this->sections.reserve(_dolPtr->secs.size());
  for (const auto& nindolsec : _dolPtr->secs) {
    this->sections.push_back(std::make_shared<DolSection>(DolSection(_dolPtr, nindolsec)));
  }
}
}