
#pragma once

#include <vector>
#include <filesystem>

#include "Binary.hpp"

#include "program_loader.pb.h"

#define PROGRAM_LOADER_PROTO_CURRENT_VERSION 1

namespace ppc2cpp {
/**
 * @brief An interface to access a program's sections, code, data, symbols and relocations.
 * Inputs can be binary, map files, or any other kind of file that provides information about a loaded program.
 * Note that this object only provides information that is obvious from the files themselves.
 * Further information is mined in the decompiler's analysis passes
*/
class ProgramLoader {
public:
  virtual ~ProgramLoader() { }
  std::vector<BinaryPtr> binaries;

  // get a location by binary name, section name and offset
  virtual std::optional<ProgramLocation> getLocation(const std::string& binaryName, const std::string& sectionName, uint32_t offset);
  virtual std::optional<uint8_t*> getBufferAtLocation(const ProgramLocation&);
  virtual std::optional<ProgramLocation> getReferenceAtLocation(const ProgramLocation&) = 0;
  // 
  virtual std::optional<ProgramLocation> resolveTarget(const ProgramLocation& pc, int32_t value, bool isAbsolute);
  // get location of virtual memory address within the program's binaries, if it exists 
  virtual std::optional<ProgramLocation> resolveVMA(uint32_t vma);

  // protobuf persistence
  persistence::ProgramLoaderType loaderType;
  void toProto(persistence::ProgramLoader*) const;
  static std::shared_ptr<ProgramLoader> fromProto(const persistence::ProgramLoader* pLoader);
};
typedef std::shared_ptr<ProgramLoader> ProgramLoaderPtr;
}