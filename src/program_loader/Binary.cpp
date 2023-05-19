
#include "ppc2cpp/program_loader/Binary.hpp"

namespace ppc2cpp {

void Binary::toProto(persistence::ProgramLoaderBinary* binaryProto) const {
  binaryProto->set_filename(this->name);
  binaryProto->set_data(this->data, this->size);
}
}