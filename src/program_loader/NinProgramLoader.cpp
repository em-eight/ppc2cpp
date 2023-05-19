
#include <fstream>

#include "ppc2cpp/common/FileTypeException.hpp"
#include "ppc2cpp/program_loader/NinProgramLoader.hpp"
#include "ppc2cpp/program_loader/Dol.hpp"
#include "ppc2cpp/program_loader/Rel.hpp"

#include "ninutils/dol.hpp"

namespace ppc2cpp {
NinProgramLoader::NinProgramLoader(const ::google::protobuf::RepeatedPtrField<persistence::ProgramLoaderBinary>& binaryProtos) {
  for (const auto& binaryProto : binaryProtos) {
    std::string filename = binaryProto.filename();
    std::string data = binaryProto.data();
    std::string file_extension = filename.substr(filename.find_last_of("."));

    if (file_extension == ".dol") {
      std::shared_ptr<ninutils::Dol> _dolptr = std::make_shared<ninutils::Dol>((uint8_t*) data.c_str(), data.size());
      this->binaries.emplace_back(std::make_shared<Dol>(filename, _dolptr));
    } else if (file_extension == ".rel") {
      std::shared_ptr<ninutils::Rel> _relptr = std::make_shared<ninutils::Rel>((uint8_t*) data.c_str(), data.size());
      this->binaries.emplace_back(std::make_shared<Rel>(filename, _relptr));
    } else {
      throw FileTypeException("Unknown extension " + file_extension);
    }
  }
}
}