
#pragma once

#include <memory>

#include "ninutils/dol.hpp"

#include "Binary.hpp"

namespace ppc2cpp {
class Dol : public Binary {
public:
  Dol(std::string name, std::shared_ptr<ninutils::Dol> dolPtr);
  virtual bool isExcecutable() const { return true; }
  virtual bool isRelocatable() const { return false; }
  virtual bool hasLoadInfo() const { return true; }

private:
  std::shared_ptr<ninutils::Dol> _dolPtr;
};
}