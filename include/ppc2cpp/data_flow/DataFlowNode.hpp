
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
  std::string label; // for visualization. A string that uniquely identifies a node (not neccessarily dot node labels)
  std::vector<VarnodePtr> inputs;

  Varnode() = default;
  Varnode(std::vector<VarnodePtr> inputs) : inputs(inputs) {}
  Varnode(std::string label) : label(label) {}
  virtual ~Varnode() = default;
};

/**
 * Varnode that is used as an explicit operand in instructions
*/
class Operandnode : public Varnode {
public:
  // index of instruction of definition or first usage within function
  uint32_t index;
  // operand index corresponding to the varnode.
  int32_t opindex;

  Operandnode(uint32_t index, int32_t opindex) : index(index), opindex(opindex) {
    this->label = "ins" + std::to_string(index) + "op" + std::to_string(opindex);
  }
  Operandnode(uint32_t index, int32_t opindex, std::string label) : index(index), opindex(opindex), Varnode(label) {}
};
typedef std::shared_ptr<Operandnode> OperandnodePtr;

/**
 * Varnode that is a "source" (does not have any inputs)
*/
class Sourcenode : public Operandnode {
public:
  Sourcenode(uint32_t index, int32_t opindex) : Operandnode(index, opindex) {
    this->label = "source_ins" + std::to_string(index) + "op" + std::to_string(opindex);
  }
};
typedef std::shared_ptr<Sourcenode> SourcenodePtr;

/**
 * Varnode that represents an immediate
*/
class Immediatenode : public Sourcenode {
public:
  Immediatenode(uint32_t index, int32_t opindex) : Sourcenode(index, opindex) {
    this->label = "imm_ins" + std::to_string(index) + "op" + std::to_string(opindex);
  }
};
typedef std::shared_ptr<Immediatenode> ImmediatenodePtr;

/**
 * Varnode that is an input to the function (register is used before defined)
*/
class InputRegnode : public Sourcenode {
public:
  InputRegnode(uint32_t index, int32_t opindex) : Sourcenode(index, opindex) {
    this->label = "reg_ins" + std::to_string(index) + "op" + std::to_string(opindex);
  }
};
typedef std::shared_ptr<InputRegnode> InputRegnodePtr;

/**
 * Varnode that stems from an intermediate result of an instruction
*/
class Resultnode : public Operandnode {
public:
  Resultnode(uint32_t index, int32_t opindex) : Operandnode(index, opindex) {
    this->label = "result_ins" + std::to_string(index) + "op" + std::to_string(opindex);
  }
};
typedef std::shared_ptr<Resultnode> ResultnodePtr;

class Sinknode : public Varnode {
public:
  // index of sink instruction
  uint32_t index;

  Sinknode(uint32_t index) : index(index) {
    this->label = "sink" + std::to_string(index);
  }
};
typedef std::shared_ptr<Sinknode> SinknodePtr;

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