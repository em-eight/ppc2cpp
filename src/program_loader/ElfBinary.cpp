
#include "ppc2cpp/program_loader/ElfBinary.hpp"
#include "ppc2cpp/program_loader/ElfSection.hpp"

using namespace ELFIO;

namespace ppc2cpp {
ElfBinary::ElfBinary(std::shared_ptr<ELFIO::elfio> elfPtr, std::string filename, std::string filedata) : _elfPtr(elfPtr) {
  this->name = name;
  this->_rawElf = filedata;
  this->size = this->_rawElf.size();
  this->data = (uint8_t*) this->_rawElf.data();

  if (_elfPtr->segments.size() > 0) {
    this->sections.reserve(_elfPtr->segments.size());
    for (int i = 0; i < _elfPtr->segments.size(); i++) {
        this->sections.push_back(std::make_shared<ElfSection>(_elfPtr, _elfPtr->segments[i]));
    }
    return;
  }

  // Non fully linked files do not have segments, so we use their sections instead
  this->sections.reserve(_elfPtr->sections.size());
  for (int i = 0; i < elfPtr->sections.size(); i++) {
    this->sections.push_back(std::make_shared<ElfSection>(elfPtr->sections[i]));
  }
  
  _hasLoadInfo = false;
  for (int i = 0; i < elfPtr->sections.size(); i++) {
    if (elfPtr->sections[i]->get_address() > 0) _hasLoadInfo = true;
  }
}

bool ElfBinary::isExcecutable() const {
  return _elfPtr->get_type() == ET_EXEC;
}

bool ElfBinary::isRelocatable() const {
  return _elfPtr->get_type() == ET_REL;
}

bool ElfBinary::hasLoadInfo() const {
  return _hasLoadInfo;
}
}