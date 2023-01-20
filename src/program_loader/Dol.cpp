
#include "ppc2cpp/program_loader/Dol.hpp"
#include "ppc2cpp/program_loader/DolSection.hpp"

namespace ppc2cpp {
Dol::Dol(std::shared_ptr<ninutils::Dol> dolPtr) : _dolPtr(dolPtr) {
  this->sections.reserve(_dolPtr->secs.size());
  for (const auto& nindolsec : _dolPtr->secs) {
    this->sections.push_back(std::make_shared<DolSection>(DolSection(_dolPtr, nindolsec)));
  }
}
}