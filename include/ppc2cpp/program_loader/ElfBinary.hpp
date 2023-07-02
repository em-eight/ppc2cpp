
#pragma once

#include <elfio/elfio.hpp>

#include "Binary.hpp"

namespace ppc2cpp {
class ElfBinary : public Binary {
public:
  ElfBinary(std::shared_ptr<ELFIO::elfio> elfPtr, std::string filename, std::string filedata);
  virtual bool isExcecutable() const override;
  virtual bool isRelocatable() const override;
  virtual bool hasLoadInfo() const override;

  const std::shared_ptr<ELFIO::elfio>& getElfPtr() const { return _elfPtr; }

private:
  std::string _rawElf;
  std::shared_ptr<ELFIO::elfio> _elfPtr;
  bool _hasLoadInfo;
};
typedef std::shared_ptr<ElfBinary> ElfBinaryPtr;
}