
#include <format>

#include "ppc2cpp/program_loader/ElfBinary.hpp"
#include "ppc2cpp/program_loader/ElfProgramLoader.hpp"

#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif


using namespace ELFIO;

namespace ppc2cpp {
ElfProgramLoader::ElfProgramLoader(const ::google::protobuf::RepeatedPtrField<persistence::ProgramLoaderBinary>& binaryProtos) {
  for (const auto& binaryProto : binaryProtos) {
    std::string filename = binaryProto.filename();
    std::string data = binaryProto.data();
    std::istringstream elf_stream(data);

    auto elfReader = std::make_shared<elfio>();
    elfReader->load(elf_stream);
    
    this->binaries.emplace_back(std::make_shared<ElfBinary>(elfReader, filename, data));

    this->registerSymbols(elfReader, this->binaries.size()-1);
  }

  this->symtab.constructNameIndex();

  for (int i = 0; i < this->binaries.size(); i++) {
    const auto& binary = static_pointer_cast<ElfBinary>(this->binaries[i]);
    this->registerRelocations(binary->getElfPtr(), i);
  }

  this->reloctab.constructSourceIndex();
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
          ProgramLocation symLocation;
          if (interpretSymValueAsAddr || section_index == SHN_ABS) {
            auto maybeSymLoc = this->resolveVMA(value);
            ASSERT(maybeSymLoc.has_value(), ("Could not resolve symbol at address " + std::format("0x{:x}", value)));
            symLocation = maybeSymLoc.value();
          } else {
            symLocation = ProgramLocation(binaryIdx, section_index, value);
          }
          this->symtab.push_back(Symbol(symLocation, size, name, static_cast<SymbolType>(type)));
        }
      }
    }
  }
}

std::string getElfSymName(const symbol_section_accessor& accessor, int sym_idx) {
  std::string name;
  Elf64_Addr value;
  Elf_Xword size;
  unsigned char bind;
  unsigned char type;
  Elf_Half section_index;
  unsigned char other;
  accessor.get_symbol( sym_idx, name, value, size, bind, type, section_index, other );
  return name;
}

void ElfProgramLoader::registerRelocations(std::shared_ptr<elfio> elfPtr, int binaryIdx) {
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
        std::string symName = getElfSymName(relsymbols, symbol_idx); // should be faster to get symbol name directly from string table
        auto symLoc = this->resolveByName(symName);
        ASSERT(symLoc.has_value(), ("Could not resolve relocation target with name " + symName + " at " + this->locationString(relocSource)));
        ProgramLocation relocDest = symLoc.value();
        reloctab.push_back(Relocation(relocSource, relocDest, type));
      }
    }
  }
}

std::optional<ProgramLocation> ElfProgramLoader::getReferenceAtLocation(const ProgramLocation& location) {
  const auto& maybeReloc = this->reloctab.lookupBySource(location);
  if (maybeReloc) return maybeReloc->destination;
  else return std::nullopt;
}
}