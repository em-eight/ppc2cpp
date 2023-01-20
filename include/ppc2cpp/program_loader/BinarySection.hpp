
#pragma once

#include <optional>
#include <cstdint>

namespace ppc2cpp {
/**
 * @brief Represents a loadable section/segment of a binary
*/
class BinarySection {
public:
  enum Type {
    SECTION_TYPE_TEXT,
    SECTION_TYPE_DATA,
    SECTION_TYPE_BSS
  };
  
  virtual std::optional<uint32_t> getAddress() const = 0;
  /**
   * @brief true if contains vma, false if not and nullopt if section relocatable and no load information
  */
  std::optional<bool> containsAddress(uint32_t vma) const { 
    auto secAddr = getAddress();
    if (!secAddr.has_value()) return std::nullopt;
    return secAddr.value() <= vma && vma < secAddr.value() + this->length;
  } 
  virtual std::string getName() const { return name; }
  virtual uint32_t getOffset() const { return offset; }
  virtual uint32_t getLength() const { return length; }
  virtual Type getType() const { return type; }

  virtual uint8_t* getBuffer() { return *getBufferAtOffset(0); }
  virtual std::optional<uint8_t*> getBufferAtOffset(uint32_t offset) const = 0;
  virtual std::optional<uint8_t*> getBufferAtAddress(uint32_t vma) const = 0;

protected:
  std::string name;
  uint32_t offset;
  uint32_t length;
  Type type;
};
typedef std::shared_ptr<BinarySection> BinarySectionPtr;
}