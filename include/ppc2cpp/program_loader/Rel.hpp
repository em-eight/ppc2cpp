
#pragma once

#include <memory>

#include "ninutils/rel.hpp"

#include "Binary.hpp"

namespace ppc2cpp {
class Rel : public Binary {
public:
  Rel(const std::filesystem::path& filepath);
  virtual bool isExcecutable() const { return false; }
  virtual bool isRelocatable() const { return true; }

  std::optional<ninutils::RelReloc> getRelocAtLocation(const BinaryLocation&);
  void genSectionName(uint8_t sec);
  int32_t getModuleId();
  void setLoadInfo(uint32_t rel_load_addr, uint32_t bss_load_addr);

private:
  std::shared_ptr<ninutils::Rel> _relPtr;
  std::unordered_map<BinaryLocation, ninutils::RelReloc, BinaryLocation::HashFunction> relocLUT;
};
typedef std::shared_ptr<Rel> RelPtr;
}