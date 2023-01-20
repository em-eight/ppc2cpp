
#include <fstream>

#include "ppc2cpp/common/FileTypeException.hpp"
#include "ppc2cpp/program_loader/NinProgramLoader.hpp"
#include "ppc2cpp/program_loader/Dol.hpp"
#include "ppc2cpp/program_loader/Rel.hpp"

#include "ninutils/dol.hpp"

namespace ppc2cpp {
NinProgramLoader::NinProgramLoader(const std::vector<std::filesystem::path>& files) {
  for (auto& file : files) {
    std::ifstream filestrm(file.c_str(), std::ios::binary | std::ios::in | std::ios::ate);
    std::streamsize size = filestrm.tellg();
    filestrm.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    filestrm.read(buffer.data(), size);

    if (file.extension() == ".dol") {
      std::shared_ptr<ninutils::Dol> _dolptr = std::make_shared<ninutils::Dol>((uint8_t*) buffer.data(), size);
      this->binaries.push_back(std::make_shared<Dol>(_dolptr));
    } else if (file.extension() == ".rel") {
      std::shared_ptr<ninutils::Rel> _relptr = std::make_shared<ninutils::Rel>((uint8_t*) buffer.data(), size);
      this->binaries.push_back(std::make_shared<Rel>(_relptr));
    } else {
      throw FileTypeException("Unknown extension " + file.extension().string());
    }
  }
}
}