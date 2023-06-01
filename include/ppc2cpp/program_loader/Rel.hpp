
#pragma once

#include <memory>

#include "ninutils/rel.hpp"

#include "Binary.hpp"

namespace ppc2cpp {
class Rel : public Binary {
public:
  Rel(std::string name, std::shared_ptr<ninutils::Rel> relPtr);
  virtual bool isExcecutable() const { return false; }
  virtual bool isRelocatable() const { return true; }
  virtual bool hasLoadInfo() const { return _relPtr->load_addr != 0 && _relPtr->bss_load_addr != 0; }

  std::optional<ninutils::RelReloc> getRelocAtLocation(const BinaryLocation&);
  void genSectionName(uint8_t sec);
  int32_t getModuleId();

private:
  std::shared_ptr<ninutils::Rel> _relPtr;
  std::unordered_map<BinaryLocation, ninutils::RelReloc, BinaryLocation::HashFunction> relocLUT;
};
typedef std::shared_ptr<Rel> RelPtr;
}