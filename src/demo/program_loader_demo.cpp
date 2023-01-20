
#include <iostream>

#include "ppc2cpp/program_loader/NinProgramLoader.hpp"

using namespace ppc2cpp;

int main(int argc, char** argv) {
  std::vector<std::filesystem::path> files;
  for (size_t i = 1; i < argc; i++) {
    files.emplace_back(argv[i]);
  }
  
  NinProgramLoader ploader(files);
  for (const auto& binary : ploader.binaries) {
    if (binary->isExcecutable()) continue; // skip DOL

    for (BinarySectionPtr sectionPtr : binary->sections) {
      std::cout << sectionPtr->getName() << std::endl;
      if (sectionPtr->getType() == BinarySection::SECTION_TYPE_TEXT) {
        uint8_t* buffer = sectionPtr->getBuffer();
      }
    }
  }
  
  return 0;
}