
#pragma once

#include <memory>
#include <vector>

namespace ppc2cpp {
class Varnode;
typedef std::shared_ptr<Varnode> VarnodePtr;

/**
 * Used in the data flow graph. A varnode represents either a source definition (immediate, function arg etc)
 * or the result of an instruction
*/
class Varnode {
public:
  std::string label; // for visualization
  std::vector<VarnodePtr> inputs;

  Varnode() = default;
  Varnode(std::vector<VarnodePtr> inputs) : inputs(inputs) {}
  virtual ~Varnode() = default;
};

class Operandnode : public Varnode {
public:
  // index of instruction of definition or first usage within function
  uint32_t index;
  // operand index corresponding to the varnode.
  int32_t opindex;
  // true if the instruction at index defined this node. False if that instruction was its first usage
  bool definition;

  Operandnode(uint32_t index, int32_t opindex, bool definition) : index(index), opindex(opindex), definition(definition) {
    this->label = "ins" + std::to_string(index) + (definition ? "out" : "in") + std::to_string(opindex);
  }
};
typedef std::shared_ptr<Operandnode> OperandnodePtr;

class Phinode : public Varnode {
public:
  Phinode(std::vector<VarnodePtr> inputs) : Varnode(inputs) {
    label = "PHI_";
    for (const auto& in : inputs) {
      label += in->label + "_";
    }
  }
};
typedef std::shared_ptr<Phinode> PhinodePtr;
}