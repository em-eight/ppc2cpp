
#pragma once

#include "DataFlowNode.hpp"
#include "FlowContext.hpp"

namespace ppc2cpp {
class DataFlowGraph {
public:
  /**
   * All variables that affect program memory or control flow
   * - Stores
   * - Branches 
   * - return values of exit blocks
   * 
   * These are the nodes that designate themselves and their dependences as used. The rest of the variables are "dead"
   * There is a vector of sinks for each basic block. Sinks must be in order of appearance in the code withing a basic block!
  */
  std::vector<std::vector<SinkNodePtr>> sinks;
  // the output machine state for each basic block
  std::vector<ppc2cpp::FlowContext> blockContexts;
};
}