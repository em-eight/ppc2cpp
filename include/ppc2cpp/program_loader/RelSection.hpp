
#pragma once

#include <memory>

#include "ninutils/rel.hpp"

#include "BinarySection.hpp"


namespace ppc2cpp {
class RelSection : public BinarySection {
public:
  RelSection(std::shared_ptr<ninutils::Rel> relPtr, ninutils::RelSection relsection);
  virtual std::optional<uint32_t> getAddress() const override;
  virtual std::optional<uint8_t*> getBufferAtOffset(uint32_t offset) const override;
  virtual std::optional<uint8_t*> getBufferAtAddress(uint32_t vma) const override;

private:
  std::shared_ptr<ninutils::Rel> _relPtr;
};
}