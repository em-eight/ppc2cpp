
#pragma once

#include <vector>
#include <filesystem>
#include <string>

#include "Binary.hpp"
#include "ppc2cpp/model/SymbolTable.hpp"
#include "ppc2cpp/model/RelocationTable.hpp"

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
  std::vector<BinaryPtr> binaries;
  SymbolTable symtab;
  RelocationTable reloctab;

  static std::shared_ptr<ProgramLoader> loadProgram(int32_t loaderProtoVersion, persistence::ProgramLoaderType loaderType,
    const std::vector<std::filesystem::path>& binaryProtos);
  virtual ~ProgramLoader() { }
  // get a location by binary name, section name and offset
  virtual std::optional<ProgramLocation> getLocation(const std::string& binaryName, const std::string& sectionName, uint32_t offset);
  virtual std::optional<uint8_t*> getBufferAtLocation(const ProgramLocation&);
  std::string locationString(const ProgramLocation&) const;
  // If the given location refers to some other part of the program, return its location, else nullopt
  virtual std::optional<ProgramLocation> getReferenceAtLocation(const ProgramLocation&) = 0;

  // If the given location refers to some other part of the program, return its location, else nullopt. For now the same as above
  // but uses given value if a relocation does not exist
  virtual std::optional<ProgramLocation> resolveTarget(const ProgramLocation& pc, int32_t value, bool isAbsolute);
  // get location of virtual memory address within the program's binaries, if it exists 
  virtual std::optional<ProgramLocation> resolveVMA(uint32_t vma);
  // get definition of symbol name
  virtual std::optional<ProgramLocation> resolveByName(const std::string& symname);
  int findBinaryIdxByName(std::string_view name) const;

  // protobuf persistence
  persistence::ProgramLoaderType loaderType;
  void toProto(persistence::ProgramLoader*) const;
  static std::shared_ptr<ProgramLoader> fromProto(const persistence::ProgramLoader* pLoader);
};
typedef std::shared_ptr<ProgramLoader> ProgramLoaderPtr;
}