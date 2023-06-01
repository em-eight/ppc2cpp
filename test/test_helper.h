
#include <gtest/gtest.h>

#include "ppc2cpp/control_flow/ControlFlowGraph.hpp"

// ik, using namespace in header files but it's very convenient and it's just for tests
using namespace ppc2cpp;

inline void EXPECT_BLOCK(const ControlFlowGraph cfg, BasicBlock block) {
  int32_t blockIdx = cfg.findBlockByPosition(block.start, block.blockType);
  ASSERT_GE(blockIdx, 0);
  EXPECT_EQ(cfg.blocks[blockIdx].blockType, block.blockType);
  EXPECT_EQ(cfg.blocks[blockIdx].start, block.start);
  EXPECT_EQ(cfg.blocks[blockIdx].end, block.end);
}