
#include <iostream>
#include <fstream>
#include <filesystem>

#include <gtest/gtest.h>

#include "ppc2cpp/model/Project.hpp"
#include "ppc2cpp/control_flow/ControlFlowAnalysis.hpp"

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