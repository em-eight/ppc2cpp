
#include <iostream>
#include <fstream>
#include <filesystem>

#include <gtest/gtest.h>

#include "ppcdisasm/ppc-dis.hpp"

#include "ppc2cpp/data_flow/DfgVisualizer.hpp"

#include "ppc2cpp/common/endian.h"
#include "ppc2cpp/model/Project.hpp"
#include "ppc2cpp/control_flow/ControlFlowAnalysis.hpp"
#include "ppc2cpp/data_flow/DataFlowAnalysis.hpp"
#include "ppc2cpp/data_flow/FlowContext.hpp"

#include "test_helper.h"

using namespace ppc2cpp;
using namespace std;

TEST(ControlFlowAnalysisTest, kartActionCalc) {
  BasicBlock expectedBlocks[] = {{0, 10, ppc2cpp::INTERNAL}, {10, 10, ppc2cpp::EXTERNAL}, {11, 13, ppc2cpp::INTERNAL},
    {25, 32, ppc2cpp::INTERNAL}, {32, 32, ppc2cpp::RUNTIME}, {14, 15, ppc2cpp::INTERNAL}, {21, 22, ppc2cpp::INTERNAL},
    {22, 22, ppc2cpp::EXTERNAL}, {23, 24, ppc2cpp::INTERNAL}, {24, 24, ppc2cpp::EXTERNAL}, {16, 18, ppc2cpp::INTERNAL},
    {19, 20, ppc2cpp::INTERNAL}};

  filesystem::path test_path = filesystem::path(TEST_PATH) / "binaries";
  // mkw binaries. TODO: test on more convenient binaries
  vector<filesystem::path> files {test_path / "main.dol", test_path / "StaticR.rel"};

  ProjectCreationOptions options;
  options.projectName = "test_project";
  options.programLoaderType = persistence::LOADER_RVL;
  options.inputFiles = files;
  options.projectFile= "./test.ppc2cpp";
  Project testProject = Project::createProject(options);

  ControlFlowAnalysis cfa(testProject.programLoader);
  // KartActionRealLocal::calc
  Function kartActionCalc(testProject.programLoader->getLocation("StaticR.rel", ".text", 0x59B78).value(), 0x80569fb0-0x80569f2c, "calc");
  cfa.functionCFA(kartActionCalc);
  const ControlFlowGraph& cfg = kartActionCalc.cfg;
  
  for (const auto& block : expectedBlocks) {
    EXPECT_BLOCK(cfg, block);
  }
}

/*void printNodeRecurse(ProgramLoaderPtr programLoader, const Function& func, VarnodePtr it) {
  if (it->inputs.size() != 0) {
    std::cout << it->label << ": ";
    if (OperandNodePtr operandIn = dynamic_pointer_cast<OperandNode>(it)) {
      // TODO: create assembly member to Function and create Disassembly analyzer
      uint32_t* funcPtr = (uint32_t*) programLoader->getBufferAtLocation(func.location).value();
      const uint32_t insn = be32(*(funcPtr + operandIn->index));
      const struct powerpc_opcode* opcode = ppcdisasm::lookup_powerpc(insn, ppcdisasm::ppc_750cl_dialect);
      std::cout << opcode->name;
    } else if (PhinodePtr operandIn = dynamic_pointer_cast<Phinode>(it)) {
      std::cout << "PHI";
    }

    std::cout << "(";
    for (const auto& in : it->inputs) {
      std::cout << in->label << ", ";
    }
    std::cout << ")" << std::endl;
    for (const auto& in : it->inputs) {
      printNodeRecurse(programLoader, func, in);
    }
  }

}*/

TEST(DataFlowAnalysisTest, kartActionCalc) {
  filesystem::path test_path = filesystem::path(TEST_PATH) / "binaries";
  // mkw binaries. TODO: test on more convenient binaries
  vector<filesystem::path> files {test_path / "main.dol", test_path / "StaticR.rel"};

  ProjectCreationOptions options;
  options.projectName = "test_project";
  options.programLoaderType = persistence::LOADER_RVL;
  options.inputFiles = files;
  options.projectFile= "./test.ppc2cpp";
  Project testProject = Project::createProject(options);

  Function quatmul(testProject.programLoader->getLocation("main.dol", ".text2", 0x8023a540-0x800072c0).value(), 0x8023a5c4-0x8023a540, "quatmul");
  ControlFlowAnalysis cfa(testProject.programLoader);
  cfa.functionCFA(quatmul);
  
  DataFlowAnalysis dfa(testProject.programLoader);
  dfa.functionDFA(quatmul);
  FlowContext& flowContext = quatmul.dfg.blockContexts[0];
  VarNodePtr outX = flowContext.fprs[0][0];

  std::cout << "f0 backwards flow" << std::endl;
  //printNodeRecurse(testProject.programLoader, quatmul, outX);
  outputDfgDot(std::cout, testProject.programLoader, quatmul);
  EXPECT_FALSE(true);
}