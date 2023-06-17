
#pragma once

#include "DataFlowNode.hpp"
#include "FlowContext.hpp"

namespace ppc2cpp {
/**
 * Heavily WIP. I think I want to have collection of out and in varnodes for each basic blocks (and also mark basic blocks as entry and exit)
 * so that liveliness analysis and {function param, stack, return value} analysis is convenient.
*/
class DataFlowGraph {
public:
  /**
   * All variables that affect program memory or control flow
   * - Stores
   * - conditional branches 
   * - function calls (branches to addresses outside of the function)
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