
#include "ppc2cpp/program_loader/Rel.hpp"
#include "ppc2cpp/program_loader/RelSection.hpp"

namespace ppc2cpp {
Rel::Rel(std::shared_ptr<ninutils::Rel> relPtr) : _relPtr(relPtr) {
  this->sections.reserve(_relPtr->secs.size());
  for (const auto& ninrelsec : _relPtr->secs) {
    this->sections.push_back(std::make_shared<RelSection>(RelSection(_relPtr, ninrelsec)));
  }
}
}