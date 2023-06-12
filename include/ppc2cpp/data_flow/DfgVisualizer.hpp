
#pragma once

#include <ostream>

#include "ppc2cpp/program_loader/ProgramLoader.hpp"
#include "ppc2cpp/model/Function.hpp"

namespace ppc2cpp {
void outputDfgDot(std::ostream& os, const ProgramLoaderPtr& ProgramLoader, const Function& func);
}