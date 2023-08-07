
#pragma once

#include <string>
#include <vector>

#include "ppc2cpp/program_loader/ProgramLoader.hpp"

namespace ppc2cpp {
struct PpcdisInfoFiles {
  std::string bin_yaml;
  std::string label_proto;
  std::string reloc_proto;
};

void loadAnalysisFromPpcdis(ProgramLoaderPtr pLoader, const std::vector<PpcdisInfoFiles>& ppcdisInfo, const std::string& symmapPath);
}