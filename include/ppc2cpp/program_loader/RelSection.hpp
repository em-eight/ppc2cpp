
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

  void setLoadInfo(uint32_t rel_load_addr, uint32_t bss_load_addr);

private:
  std::shared_ptr<ninutils::Rel> _relPtr;
  // As in ninutils, a value of zero means unknown load address
  uint32_t load_address;
};
typedef std::shared_ptr<RelSection> RelSectionPtr;
}