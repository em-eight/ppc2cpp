
#pragma once

#include <memory>
#include <vector>

#include "ppc2cpp/model/CpuMemory.hpp"

namespace ppc2cpp {
class VarNode;
typedef std::shared_ptr<VarNode> VarNodePtr;

/**
 * Used in the data flow graph. A varnode represents either a source definition (immediate, function arg etc)
 * or the result of an instruction
*/
class VarNode {
public:
  std::vector<VarNodePtr> inputs;

  VarNode() = default;
  VarNode(std::vector<VarNodePtr> inputs) : inputs(inputs) {}
  virtual ~VarNode() = default;
};

/**
 * A varnode in a data flow graph.
*/
class DataFlowNode : public VarNode {
public:
  // index of an instruction associated with the node. It can be the instruction that defined the variable, or its first usage
  uint32_t index;

  DataFlowNode(uint32_t index) : index(index) {}
};
typedef std::shared_ptr<DataFlowNode> DataFlowNodePtr;

/**
 * A varnode produced by one operand in an instruction's use-define information
*/
class OperandNode : public DataFlowNode {
public:
  // index of an instruction associated with the node. It can be the instruction that defined the variable, or its first usage
  uint32_t opindex;

  OperandNode(uint32_t index, uint32_t opindex) : DataFlowNode(index), opindex(opindex) {}
};
typedef std::shared_ptr<OperandNode> OperandNodePtr;

/**
 * Varnode that is tied to a register value at some point in the program
*/
class RegisterNode : public DataFlowNode {
public:
  CpuMemoryLocation cpuMemoryLocation;

  RegisterNode(uint32_t index, CpuMemoryLocation cpuMemoryLocation) : DataFlowNode(index), cpuMemoryLocation(cpuMemoryLocation) {}
};
typedef std::shared_ptr<RegisterNode> RegisterNodePtr;

/**
 * Varnode that is an input to a function (the function does not define it but uses it)
*/
class FunctionInputNode : public RegisterNode {
public:
  FunctionInputNode(uint32_t index, CpuMemoryLocation cpuMemoryLocation) : RegisterNode(index, cpuMemoryLocation) {}
};
typedef std::shared_ptr<FunctionInputNode> FunctionInputNodePtr;

/**
 * Varnode that is used as an explicit operand in instructions
*/
class RegisterOperandNode : public OperandNode {
public:
  CpuMemoryLocation cpuMemoryLocation;

  RegisterOperandNode(uint32_t index, uint32_t opindex, CpuMemoryLocation cpuMemoryLocation) : OperandNode(index, opindex), cpuMemoryLocation(cpuMemoryLocation) {}
};
typedef std::shared_ptr<RegisterOperandNode> RegisterOperandNodePtr;

/**
 * Varnode that represents an immediate
*/
class ImmediateNode : public OperandNode {
public:
  int64_t value;

  ImmediateNode(uint32_t index, uint32_t opindex, int64_t value) : OperandNode(index, opindex), value(value) {}
};
typedef std::shared_ptr<ImmediateNode> ImmediateNodePtr;

/**
 * Varnode that is returned by a called function
*/
class CallReturnNode : public RegisterNode {
public:

  CallReturnNode(uint32_t index, CpuMemoryLocation cpuMemoryLocation) : RegisterNode(index, cpuMemoryLocation) {}
};
typedef std::shared_ptr<CallReturnNode> CallReturnNodePtr;

/**
 * Varnode that stems from an intermediate result of an instruction
*/
class ResultNode : public RegisterOperandNode {
public:
  ResultNode(uint32_t index, uint32_t opindex, CpuMemoryLocation cpuMemoryLocation) : RegisterOperandNode(index, opindex, cpuMemoryLocation) {}
};
typedef std::shared_ptr<ResultNode> ResultNodePtr;

/**
 * A data flow node that represents a sink instruction (instruction that uses its inputs but has no usages by other nodes)
*/
class SinkNode : public DataFlowNode {
public:
  SinkNode(uint32_t index) : DataFlowNode(index) {}
};
typedef std::shared_ptr<SinkNode> SinkNodePtr;

class ReturnNode : public SinkNode {
public:
  CpuMemoryLocation cpuMemoryLocation;

  ReturnNode(uint32_t index, CpuMemoryLocation cpuMemoryLocation) : SinkNode(index), cpuMemoryLocation(cpuMemoryLocation) {}
};
typedef std::shared_ptr<ReturnNode> ReturnNodePtr;

class PhiNode : public RegisterNode {
public:
  PhiNode(uint32_t index, CpuMemoryLocation cpuMemoryLocation) : RegisterNode(index, cpuMemoryLocation) {}
};
typedef std::shared_ptr<PhiNode> PhiNodePtr;
}