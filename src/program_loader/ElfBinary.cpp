
#include "ppc2cpp/program_loader/ElfBinary.hpp"
#include "ppc2cpp/program_loader/ElfSection.hpp"

using namespace ELFIO;

namespace ppc2cpp {
ElfBinary::ElfBinary( const std::filesystem::path& filepath) : Binary(filepath) {
  std::istringstream elf_stream(this->data);
  this->_elfPtr = std::make_shared<elfio>();
  this->_elfPtr->load(elf_stream);

  if (_elfPtr->segments.size() > 0) {
    this->sections.reserve(_elfPtr->segments.size());
    for (int i = 0; i < _elfPtr->segments.size(); i++) {
        this->sections.push_back(std::make_shared<ElfSection>(_elfPtr, _elfPtr->segments[i]));
    }
    return;
  }

  // Non fully linked files do not have segments, so we use their sections instead
  this->sections.reserve(_elfPtr->sections.size());
  for (int i = 0; i < _elfPtr->sections.size(); i++) {
    this->sections.push_back(std::make_shared<ElfSection>(_elfPtr->sections[i]));
  }
}

bool ElfBinary::isExcecutable() const {
  return _elfPtr->get_type() == ET_EXEC;
}

bool ElfBinary::isRelocatable() const {
  return _elfPtr->get_type() == ET_REL;
}
}