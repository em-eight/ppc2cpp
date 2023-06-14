
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
 * Varnode that is used as an explicit operand in instructions
*/
class RegisterNode : public OperandNode {
public:
  CpuMemoryLocation cpuMemoryLocation;

  RegisterNode(uint32_t index, uint32_t opindex, CpuMemoryLocation cpuMemoryLocation) : OperandNode(index, opindex), cpuMemoryLocation(cpuMemoryLocation) {}
};
typedef std::shared_ptr<RegisterNode> RegisterNodePtr;

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
 * Varnode that is an input to the function (register is used before defined)
*/
class InputRegisterNode : public RegisterNode {
public:

  InputRegisterNode(uint32_t index, uint32_t opindex, CpuMemoryLocation cpuMemoryLocation) : RegisterNode(index, opindex, cpuMemoryLocation) {}
};
typedef std::shared_ptr<InputRegisterNode> InputRegisterNodePtr;

/**
 * Varnode that stems from an intermediate result of an instruction
*/
class ResultNode : public RegisterNode {
public:
  ResultNode(uint32_t index, uint32_t opindex, CpuMemoryLocation cpuMemoryLocation) : RegisterNode(index, opindex, cpuMemoryLocation) {}
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

class PhiNode : public OperandNode {
public:
  PhiNode(uint32_t index, uint32_t opindex) : OperandNode(index, opindex) {}
};
typedef std::shared_ptr<PhiNode> PhiNodePtr;
}