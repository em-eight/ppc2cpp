
#pragma once

#include <optional>
#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>

#include "BinarySection.hpp"
#include "ppc2cpp/model/ProgramLocation.hpp"

#include "program_loader.pb.h"

namespace ppc2cpp {
class Binary {
public:
  // classes inheriting Binary should call this superconstructor to initialize Binary's data, size and name
  Binary(std::filesystem::path filepath);

  virtual bool isExcecutable() const = 0;
  virtual bool isRelocatable() const = 0;
  /**
   * @brief true if contains vma, false if not and nullopt if section relocatable and no load information
  */
  std::optional<bool> containsAddress(uint32_t vma) const { 
    for (const auto& sec : this->sections)
      if (sec->containsAddress(vma)) return true;
    
    return false;
  } 

  // try to get section by VMA
  virtual std::optional<BinarySectionPtr> getSectionByAddress(uint32_t vma) const {
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

  virtual std::optional<uint8_t*> getBufferAtLocation(const BinaryLocation& location) const {
    return sections[location.section_idx]->getBufferAtOffset(location.section_offset);
  }

  virtual std::optional<uint8_t*> getBufferAtOffset(const BinarySection& section, uint32_t offset) const {
    return section.getBufferAtOffset(offset);
  }

  virtual std::optional<uint8_t*> getBufferAtAddress(uint32_t vma) const {
    for (const auto& sec : this->sections) {
      auto maybeBuffer = sec->getBufferAtAddress(vma);
      if (maybeBuffer.has_value()) return maybeBuffer;
    }
    return std::nullopt;
  }

  // get location of virtual memory address within the program's binaries, if it exists 
  virtual std::optional<BinaryLocation> resolveVMA(uint32_t vma);

  std::filesystem::path filepath;
  // original binary filename
  std::string name;
  // buffer to file data
  std::string data;
  // sections/segments of a binary
  std::vector<std::shared_ptr<BinarySection>> sections;

  // proto persistence
  void toProto(persistence::ProgramLoaderBinary*) const;
};
typedef std::shared_ptr<Binary> BinaryPtr;
}