
#pragma once

#include <memory>

#include "ninutils/dol.hpp"

#include "BinarySection.hpp"

namespace ppc2cpp {
class DolSection : public BinarySection {
public:
  DolSection(std::shared_ptr<ninutils::Dol> relPtr, ninutils::DolSection relsection);
  virtual std::optional<uint32_t> getAddress() const override;
  virtual std::optional<uint8_t*> getBufferAtOffset(uint32_t offset) const override;
  virtual std::optional<uint8_t*> getBufferAtAddress(uint32_t vma) const override;

private:
  std::shared_ptr<ninutils::Dol> _dolPtr;
  uint32_t address;
};
}