
#pragma once

#include <functional>
#include <unordered_map>

#include "ppc2cpp/program_loader/ProgramLoader.hpp"
#include "ppc2cpp/model/Function.hpp"

namespace ppc2cpp {
// Caches the result of comparison between varnodes
class VarNodeComparisonCache {
  using NodePair = std::pair<VarNodePtr, VarNodePtr>;
  struct VarNodeCacheHash {
    std::size_t operator()(const NodePair& pair) const {
      return (std::hash<VarNode*>{}(pair.first.get())) ^ (std::hash<VarNode*>{}(pair.second.get()));
    }

  };
  std::unordered_map<NodePair, bool, VarNodeCacheHash> _cache;

public:
  void set(VarNodePtr node1, VarNodePtr node2, bool equal) {
    _cache.insert_or_assign({node1, node2}, equal);
  }
  std::optional<bool> check(VarNodePtr node1, VarNodePtr node2) {
    if (_cache.contains({node1, node2})) {
      return _cache.at({node1, node2});
    } else return std::nullopt;
  }
};

class ProgramComparator {
public:
  ProgramLoaderPtr pLoader1;
  ProgramLoaderPtr pLoader2;

  ProgramComparator(ProgramLoaderPtr pLoader1, ProgramLoaderPtr pLoader2) : pLoader1(pLoader1), pLoader2(pLoader2) {}

  bool comparePrograms();
  bool compareRelocations(const Relocation& reloc1, const Relocation& reloc2, const std::string& symName, int off);
  bool compareData(const Symbol& sourceSym, const Symbol& targetSym);
  bool compareFunctions(const Symbol& sourceSym, const Symbol& targetSym);
  bool compareSymbols(const Symbol& sourceSym, const Symbol& targetSym);
  bool compareFunctionFlows(const Function& func1, const Function& func2);

private:
  uint32_t* func1ptr;
  uint32_t* func2ptr;

  bool compareVarNodes(const Function& func1, const Function& func2, const VarNodePtr& var1, const VarNodePtr& var2, VarNodeComparisonCache& comparisonCache);
};
}