
#pragma once

#include <elfio/elfio.hpp>

#include "Binary.hpp"

namespace ppc2cpp {
class ElfBinary : public Binary {
public:
  ElfBinary(const std::filesystem::path& filepath);
  virtual bool isExcecutable() const override;
  virtual bool isRelocatable() const override;

  const std::shared_ptr<ELFIO::elfio>& getElfPtr() const { return _elfPtr; }

private:
  std::shared_ptr<ELFIO::elfio> _elfPtr;
};
typedef std::shared_ptr<ElfBinary> ElfBinaryPtr;
}