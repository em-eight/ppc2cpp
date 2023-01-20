
#pragma once

#include <memory>

#include "ninutils/rel.hpp"

#include "Binary.hpp"

namespace ppc2cpp {
class Rel : public Binary {
public:
  Rel(std::shared_ptr<ninutils::Rel> relPtr);
  virtual bool isExcecutable() const { return false; }
  virtual bool isRelocatable() const { return true; }
  virtual bool hasLoadInfo() const { return _relPtr->load_addr != 0 && _relPtr->bss_load_addr != 0; }

private:
  std::shared_ptr<ninutils::Rel> _relPtr;
};
}