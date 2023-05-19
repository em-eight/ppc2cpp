
#pragma once

#include <optional>
#include <cstdint>
#include <string>
#include <vector>

#include "BinarySection.hpp"

#include "program_loader.pb.h"

namespace ppc2cpp {
class Binary {
public:
  virtual bool isExcecutable() const = 0;
  virtual bool isRelocatable() const = 0;
  /**
   * @brief If the binary is relocatable, whether we have load information for it or not
  */
  virtual bool hasLoadInfo() const = 0;
  /**
   * @brief true if contains vma, false if not and nullopt if section relocatable and no load information
  */
  std::optional<bool> containsAddress(uint32_t vma) const { 
    if (!hasLoadInfo()) return std::nullopt;
    for (const auto& sec : this->sections)
      if (sec->containsAddress(vma)) return true;
    
    return false;
  } 

  // try to get section by VMA
  virtual std::optional<BinarySectionPtr> getSectionByAddress(uint32_t vma) const {
    if (!hasLoadInfo()) return std::nullopt;
    const uint32_t numSections = this->sections.size();
    for (const auto & sec : this->sections) {
      auto maybeBuffer = sec->getBufferAtAddress(vma);
      if (maybeBuffer.has_value()) return sec;
    }
    return std::nullopt;

  }

  virtual std::optional<BinarySectionPtr> getSectionIdxByName(std::string name) const {
    const uint32_t numSections = this->sections.size();
    for (const auto & sec : this->sections)
      if (sec->getName() == name) return sec;
    return std::nullopt;
  }

  virtual std::optional<uint8_t*> getBufferAtOffset(const BinarySection& section, uint32_t offset) const {
    return section.getBufferAtOffset(offset);
  }

  virtual std::optional<uint8_t*> getBufferAtAddress(uint32_t vma) const {
    if (!hasLoadInfo()) return std::nullopt;
    for (const auto& sec : this->sections) {
      auto maybeBuffer = sec->getBufferAtAddress(vma);
      if (maybeBuffer.has_value()) return maybeBuffer;
    }
    return std::nullopt;
  }

  // original binary filename
  std::string name;
  // buffer to file data
  uint8_t* data;
  // size of file in bytes
  uint32_t size;
  // sections/segments of a binary
  std::vector<std::shared_ptr<BinarySection>> sections;

  // proto persistence
  void toProto(persistence::ProgramLoaderBinary*) const;
};
typedef std::shared_ptr<Binary> BinaryPtr;
}