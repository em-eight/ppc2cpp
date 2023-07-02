
#pragma once

#include <memory>

#include <elfio/elfio.hpp>

#include "BinarySection.hpp"

namespace ppc2cpp {
class ElfSection : public BinarySection {
public:
  ElfSection(const ELFIO::section* elfsec);
  ElfSection(std::shared_ptr<ELFIO::elfio> elfPtr, const ELFIO::segment* elfseg);
  virtual std::optional<uint32_t> getAddress() const override;
  virtual std::optional<uint8_t*> getBufferAtOffset(uint32_t offset) const override;
  virtual std::optional<uint8_t*> getBufferAtAddress(uint32_t vma) const override;

private:
  uint8_t* _data;
  uint32_t address;
};
}