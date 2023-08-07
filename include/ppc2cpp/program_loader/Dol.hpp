
#pragma once

#include <memory>

#include "ninutils/dol.hpp"

#include "Binary.hpp"

namespace ppc2cpp {
class Dol : public Binary {
public:
  Dol(const std::filesystem::path& filepath);
  virtual bool isExcecutable() const { return true; }
  virtual bool isRelocatable() const { return false; }

private:
  std::shared_ptr<ninutils::Dol> _dolPtr;
};
typedef std::shared_ptr<Dol> DolPtr;
}