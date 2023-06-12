
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
  //std::vector<VarnodePtr> nodes;
  // the output machine state for each basic block
  std::vector<ppc2cpp::FlowContext> blockContexts;
};
}