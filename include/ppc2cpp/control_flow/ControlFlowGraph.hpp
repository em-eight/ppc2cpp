
#pragma once

#include <cstdint>
#include <vector>
#include <set>

namespace ppc2cpp {
enum BlockType {
  // block inside function
  INTERNAL,
  // block in other function, known at compile time
  EXTERNAL,
  // block in other function, determined at runtime
  RUNTIME
};

class BasicBlock {
public:
  // Offset from function start. The branch target of the previous block
  uint32_t start;
  // Offset from function start. The address of the function that branches to another basic block
  uint32_t end;
  // block type. start and end are only valid for INTERNAL BlockType
  BlockType blockType;

  bool isExit = false;
  bool endsInCall = false;
  bool endsInTailCall = false;

  BasicBlock(uint32_t start) : start(start), end(start), blockType(INTERNAL) {}
  BasicBlock(uint32_t start, uint32_t end) : start(start), end(end), blockType(INTERNAL) {}
  BasicBlock(uint32_t start, uint32_t end, BlockType blockType) : start(start), end(end), blockType(blockType) {}
  // ctor for non internal blocks
  BasicBlock(BlockType blockType, uint32_t start) : blockType(blockType), start(start), end(start) {}
};

class PSEntry {
public:
  std::vector<uint32_t> pred;
  std::vector<uint32_t> succ; // lmao
};

class ControlFlowGraph {
public:
  std::vector<BasicBlock> blocks;
  // predecessor-succesor table. For each basic block i contains the index of all its predecessors and successors
  std::vector<PSEntry> psTable;

  void init() {
    blocks.emplace_back(0);
    psTable.push_back(PSEntry());
  }
  
  // checks if block exists and returns index, if not found, return -1
  int32_t findBlockByPosition(uint32_t branchTarget, BlockType blockType) const {
    for (uint32_t i = 0; i < blocks.size(); i++) {
      if (blocks[i].start == branchTarget && blocks[i].blockType == blockType) return i;
    }
    return -1;
  }

  int32_t findInternalBlockByPosition(uint32_t branchTarget) const {
    return findBlockByPosition(branchTarget, INTERNAL);
  }
  
  // Returns the index of the successor if a new one was created, else -1
  uint32_t tryCreateSuccessorInternal(uint32_t originBlockIdx, uint32_t branchPos, uint32_t targetPosition) {
    blocks[originBlockIdx].end = branchPos;
    int32_t existingBlock = findInternalBlockByPosition(targetPosition);
    if (existingBlock >= 0) {
      psTable.push_back(PSEntry());
      psTable[originBlockIdx].succ.push_back(existingBlock);
      psTable[existingBlock].pred.push_back(originBlockIdx);
      return -1;
    } else {
      uint32_t blockIdx = blocks.size();
      blocks.emplace_back(targetPosition);

      psTable.push_back(PSEntry());
      psTable[originBlockIdx].succ.push_back(blockIdx);
      psTable[blockIdx].pred.push_back(originBlockIdx);
      return blockIdx;
    }
  }

  void createSuccessorExternal(uint32_t originBlockIdx, uint32_t branchPos) {
    blocks[originBlockIdx].end = branchPos;
    uint32_t blockIdx = blocks.size();
    blocks.emplace_back(EXTERNAL, branchPos);

    psTable.push_back(PSEntry());
    psTable[originBlockIdx].succ.push_back(blockIdx);
    psTable[blockIdx].pred.push_back(originBlockIdx);
  }

  void createSuccessorRuntime(uint32_t originBlockIdx, uint32_t branchPos) {
    blocks[originBlockIdx].end = branchPos;
    uint32_t blockIdx = blocks.size();
    blocks.emplace_back(RUNTIME, branchPos);

    psTable.push_back(PSEntry());
    psTable[originBlockIdx].succ.push_back(blockIdx);
    psTable[blockIdx].pred.push_back(originBlockIdx);
  }
  
  void createSuccessorFallthrough(uint32_t originBlockIdx, uint32_t pos, uint32_t fallthroughBlockIdx) {
    blocks[originBlockIdx].end = pos;

    psTable[originBlockIdx].succ.push_back(fallthroughBlockIdx);
    psTable[fallthroughBlockIdx].pred.push_back(originBlockIdx);
  }

  bool hasExternalSuccessor(uint32_t blockIdx) {
    for (auto successor : psTable[blockIdx].succ) {
      if (blocks[successor].blockType != INTERNAL) return true;
    }
    return false;
  }
};
}