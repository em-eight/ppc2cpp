#include <fmt/format.h>
#include <sstream>

#include "ppc2cpp/common/common.h"
#include "ppc2cpp/program_loader/ElfBinary.hpp"
#include "ppc2cpp/program_loader/ElfProgramLoader.hpp"


using namespace ELFIO;

namespace ppc2cpp {
ElfProgramLoader::ElfProgramLoader(const std::vector<std::filesystem::path>& binaryPaths) {
  for (const auto& binaryPath : binaryPaths) {
    ElfBinaryPtr elfBinary = std::make_shared<ElfBinary>(binaryPath);
    this->binaries.emplace_back(elfBinary);

    this->registerSymbols(elfBinary->getElfPtr(), this->binaries.size()-1);
  }

  this->symtab.constructNameIndex();
  this->symtab.constructLocationIndex();

  for (int i = 0; i < this->binaries.size(); i++) {
    const auto& binary = static_pointer_cast<ElfBinary>(this->binaries[i]);
    this->registerRelocations(binary->getElfPtr(), i);
  }

  this->reloctab.constructSourceIndex();
}

ProgramLocation ElfProgramLoader::getElfSymbolLocation(int binaryIdx, Elf_Half section_index, Elf64_Addr value, bool interpretSymValueAsAddr) {
  if (interpretSymValueAsAddr || section_index == SHN_ABS) {
    auto maybeSymLoc = this->resolveVMA(value);
    ASSERT(maybeSymLoc.has_value(), ("Could not resolve symbol at address " + fmt::format("0x{:x}", value)));
    return maybeSymLoc.value();
  } else {
    return ProgramLocation(binaryIdx, section_index, value);
  }
}

void ElfProgramLoader::registerSymbols(std::shared_ptr<elfio> elfPtr, int binaryIdx) {
  bool interpretSymValueAsAddr = elfPtr->get_type() != ET_REL;

  for ( int i = 0; i < elfPtr->sections.size(); ++i ) {
    const section* psec = elfPtr->sections[i];

    if ( psec->get_type() == SHT_SYMTAB ) {
      const symbol_section_accessor symbols( *elfPtr, const_cast<section*>(psec) );
      for ( unsigned int j = 0; j < symbols.get_symbols_num(); ++j ) {
        std::string name;
        Elf64_Addr value;
        Elf_Xword size;
        unsigned char bind;
        unsigned char type;
        Elf_Half section_index;
        unsigned char other;
        symbols.get_symbol( j, name, value, size, bind, type, section_index, other );

        if (section_index != SHN_UNDEF && (section_index == SHN_ABS || section_index < SHN_LORESERVE) && (type == STT_OBJECT || type == STT_FUNC || type == STT_NOTYPE)) {
          ProgramLocation symLocation = getElfSymbolLocation(binaryIdx, section_index, value, interpretSymValueAsAddr);
          this->symtab.push_back(Symbol(symLocation, size, name, static_cast<SymbolType>(type), static_cast<SymbolBinding>(bind)));
        }
      }
    }
  }
}

ProgramLocation ElfProgramLoader::getElfRelocDest(const symbol_section_accessor& accessor, int sym_idx, int binaryIdx,
    const ProgramLocation& relocSource, bool interpretSymValueAsAddr) {
  std::string name;
  Elf64_Addr value;
  Elf_Xword size;
  unsigned char bind;
  unsigned char type;
  Elf_Half section_index;
  unsigned char other;
  accessor.get_symbol( sym_idx, name, value, size, bind, type, section_index, other );
  if (section_index != SHN_UNDEF) {
    return this->getElfSymbolLocation(binaryIdx, section_index, value, interpretSymValueAsAddr);
  } else {
    auto symLoc = this->resolveByName(name);
    if (symLoc.has_value()) {
      return symLoc.value();
    } else {
      // reference to undefined symbol
#ifndef NDEBUG
      std::cout << "Warning: Reference to undefined symbol " << name << " at " << this->locationString(relocSource) << '\n';
#endif
      return ProgramLocation(0, 0, 0);
    }
  }
}

void ElfProgramLoader::registerRelocations(std::shared_ptr<elfio> elfPtr, int binaryIdx) {
  bool interpretSymValueAsAddr = elfPtr->get_type() != ET_REL;

  for ( int i = 0; i < elfPtr->sections.size(); ++i ) {
    const section* psec = elfPtr->sections[i];

    if (psec->get_type() == SHT_RELA) {
      const relocation_section_accessor relocs( *elfPtr, const_cast<section*>(psec) );

      int32_t rel_sec_idx = psec->get_info(); // section index of related section
      uint32_t rel_sym_idx = psec->get_link(); // section index of related symbol table
      const section* elfSymTab = elfPtr->sections[rel_sym_idx];
      const symbol_section_accessor relsymbols( *elfPtr, const_cast<section*>(elfSymTab) );

      for ( unsigned int j = 0; j < relocs.get_entries_num(); ++j ) {
        Elf64_Addr offset;
        Elf_Word symbol_idx;
        Elf_Word type;
        Elf_Sxword addend;

        relocs.get_entry(j, offset, symbol_idx, type, addend);
        ProgramLocation relocSource = { binaryIdx, rel_sec_idx, static_cast<uint32_t>(offset) };
        ProgramLocation relocDest = this->getElfRelocDest(relsymbols, symbol_idx, binaryIdx, relocSource, interpretSymValueAsAddr);
        reloctab.push_back(Relocation(relocSource, relocDest, type, addend));
      }
    }
  }
}

std::optional<ProgramLocation> ElfProgramLoader::getReferenceAtLocation(const ProgramLocation& location) {
  const auto& maybeReloc = this->reloctab.lookupBySource(location);
  if (maybeReloc) return maybeReloc->destination;
  else return std::nullopt;
}
  
bool ElfProgramLoader::isElfProgram(const std::vector<std::filesystem::path>& files) {
  constexpr char elfmagic[4] = {0x7f, 'E', 'L', 'F'};
  char magic[4];
  for (const auto& file : files) {
    std::ifstream filestream(file);
    filestream.read(magic, sizeof(magic));
    if (std::memcmp(elfmagic, magic, sizeof(magic)) != 0)
      return false;
  }
  return true;
}
}