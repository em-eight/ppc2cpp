
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
#include "ppc2cpp/analysis/ProgramComparator.hpp"

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

TEST(ProgramLoaderTest, ElfRvlEquivalence) {
  filesystem::path test_path = filesystem::path(TEST_PATH) / "binaries";
  vector<filesystem::path> files {test_path / "main.dol", test_path / "StaticR.rel"};

  ProjectCreationOptions options;
  options.projectName = "test_project";
  options.programLoaderType = persistence::LOADER_RVL;
  options.inputFiles = files;
  options.projectFile= "./test.ppc2cpp";
  Project testProject = Project::createProject(options);

  vector<filesystem::path> filesElf {test_path / "main.elf", test_path / "StaticR.elf"};

  ProjectCreationOptions optionsElf;
  optionsElf.projectName = "test_project_elf";
  optionsElf.programLoaderType = persistence::LOADER_ELF;
  optionsElf.inputFiles = filesElf;
  optionsElf.projectFile= "./testElf.ppc2cpp";
  Project testProjectElf = Project::createProject(optionsElf);
  
  auto size = 0x8023a5c4-0x8023a540;
  ProgramLocation quatmulRvl = testProject.programLoader->getLocation("main.dol", ".text2", 0x8023a540-0x800072c0).value();
  ProgramLocation quatmulElf = testProjectElf.programLoader->resolveByName("__ml__3EGGFRCQ23EGG5QuatfRCQ23EGG8Vector3f").value();

  ASSERT_TRUE(testProject.programLoader->getBufferAtLocation(quatmulRvl).has_value());
  ASSERT_TRUE(testProjectElf.programLoader->getBufferAtLocation(quatmulElf).has_value());
  uint8_t* quatmulRvlBuf = testProject.programLoader->getBufferAtLocation(quatmulRvl).value();
  uint8_t* quatmulElfBuf = testProjectElf.programLoader->getBufferAtLocation(quatmulElf).value();


  EXPECT_EQ(0, std::memcmp(quatmulElfBuf, quatmulRvlBuf, size));
}

TEST(DataFlowAnalysisTest, FunctionEquivalence) {
  filesystem::path test_path = filesystem::path(TEST_PATH) / "binaries";
  vector<filesystem::path> files1 {test_path / "main.elf", test_path / "StaticR.elf"};

  ProjectCreationOptions options1;
  options1.projectName = "test_project1";
  options1.programLoaderType = persistence::LOADER_ELF;
  options1.inputFiles = files1;
  options1.projectFile= "./test1.ppc2cpp";
  Project testProject1 = Project::createProject(options1);

  vector<filesystem::path> files2 {test_path / "main2.elf", test_path / "StaticR.elf"};

  ProjectCreationOptions options2;
  options2.projectName = "test_project2";
  options2.programLoaderType = persistence::LOADER_ELF;
  options2.inputFiles = files2;
  options2.projectFile= "./test2.ppc2cpp";
  Project testProject2 = Project::createProject(options2);
  
  auto size = 0x80239f58-0x80239e10;
  ProgramLocation setRPY1loc = testProject1.programLoader->resolveByName("setRPY__Q23EGG5QuatfFRCQ23EGG8Vector3f").value();
  ProgramLocation setRPY2loc = testProject2.programLoader->resolveByName("setRPY__Q23EGG5QuatfFRCQ23EGG8Vector3f").value();

  ASSERT_TRUE(testProject1.programLoader->getBufferAtLocation(setRPY1loc).has_value());
  ASSERT_TRUE(testProject2.programLoader->getBufferAtLocation(setRPY2loc).has_value());

  Function setRPY1(setRPY1loc, size, "setRPY");
  ControlFlowAnalysis cfa1(testProject1.programLoader);
  cfa1.functionCFA(setRPY1);
  DataFlowAnalysis dfa1(testProject1.programLoader);
  dfa1.functionDFA(setRPY1);
  Function setRPY2(setRPY2loc, size, "setRPY");
  ControlFlowAnalysis cfa2(testProject2.programLoader);
  cfa2.functionCFA(setRPY2);
  DataFlowAnalysis dfa2(testProject2.programLoader);
  dfa2.functionDFA(setRPY2);

  ProgramComparator programComparator(testProject1.programLoader, testProject2.programLoader);
  bool areEquivalent = programComparator.compareFunctionFlows(setRPY1, setRPY2);
}

TEST(DataFlowAnalysisTest, FunctionEquivalenceSqNorm) {
  filesystem::path test_path = filesystem::path(TEST_PATH) / "binaries";
  vector<filesystem::path> files1 {test_path / "main.elf", test_path / "StaticR.elf"};

  ProjectCreationOptions options1;
  options1.projectName = "test_project1";
  options1.programLoaderType = persistence::LOADER_ELF;
  options1.inputFiles = files1;
  options1.projectFile= "./test1.ppc2cpp";
  Project testProject1 = Project::createProject(options1);

  vector<filesystem::path> files2 {test_path / "main2.elf", test_path / "StaticR.elf"};

  ProjectCreationOptions options2;
  options2.projectName = "test_project2";
  options2.programLoaderType = persistence::LOADER_ELF;
  options2.inputFiles = files2;
  options2.projectFile= "./test2.ppc2cpp";
  Project testProject2 = Project::createProject(options2);
  
  auto size = 0x8023a168-0x8023a138;
  ProgramLocation sqNorm1loc = testProject1.programLoader->resolveByName("squareNorm__Q23EGG5QuatfFv").value();
  ProgramLocation sqNorm2loc = testProject2.programLoader->resolveByName("squareNorm__Q23EGG5QuatfFv").value();

  ASSERT_TRUE(testProject1.programLoader->getBufferAtLocation(sqNorm1loc).has_value());
  ASSERT_TRUE(testProject2.programLoader->getBufferAtLocation(sqNorm2loc).has_value());

  Function sqNorm1(sqNorm1loc, size, "sqNorm");
  ControlFlowAnalysis cfa1(testProject1.programLoader);
  cfa1.functionCFA(sqNorm1);
  DataFlowAnalysis dfa1(testProject1.programLoader);
  dfa1.functionDFA(sqNorm1);
  Function sqNorm2(sqNorm2loc, size, "sqNorm");
  ControlFlowAnalysis cfa2(testProject2.programLoader);
  cfa2.functionCFA(sqNorm2);
  DataFlowAnalysis dfa2(testProject2.programLoader);
  dfa2.functionDFA(sqNorm2);

  ProgramComparator programComparator(testProject1.programLoader, testProject2.programLoader);
  bool areEquivalent = programComparator.compareFunctionFlows(sqNorm1, sqNorm2);

  EXPECT_TRUE(areEquivalent);
}

TEST(DataFlowAnalysisTest, quatmul) {
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
}

TEST(DataFlowAnalysisTest, canHop) {
  filesystem::path test_path = filesystem::path(TEST_PATH) / "binaries";
  // mkw binaries. TODO: test on more convenient binaries
  vector<filesystem::path> files {test_path / "main.dol", test_path / "StaticR.rel"};

  ProjectCreationOptions options;
  options.projectName = "test_project";
  options.programLoaderType = persistence::LOADER_RVL;
  options.inputFiles = files;
  options.projectFile= "./test.ppc2cpp";
  Project testProject = Project::createProject(options);

  Function kartActionCalc(testProject.programLoader->getLocation("StaticR.rel", ".text", 0x8057da18-0x805103b4).value(), 0x8057da5c-0x8057da18, "canHop");
  ControlFlowAnalysis cfa(testProject.programLoader);
  cfa.functionCFA(kartActionCalc);
  
  DataFlowAnalysis dfa(testProject.programLoader);
  dfa.functionDFA(kartActionCalc);

  //outputDfgDot(std::cout, testProject.programLoader, kartActionCalc);
  //EXPECT_FALSE(true);
}

TEST(DataFlowAnalysisTest, canAirtimeHop) {
  filesystem::path test_path = filesystem::path(TEST_PATH) / "binaries";
  // mkw binaries. TODO: test on more convenient binaries
  vector<filesystem::path> files {test_path / "main.dol", test_path / "StaticR.rel"};

  ProjectCreationOptions options;
  options.projectName = "test_project";
  options.programLoaderType = persistence::LOADER_RVL;
  options.inputFiles = files;
  options.projectFile= "./test.ppc2cpp";
  Project testProject = Project::createProject(options);

  Function airtimeHop(testProject.programLoader->getLocation("StaticR.rel", ".text", 0x8057d888-0x805103b4).value(), 0x8057da18-0x8057d888, "canHop");
  ControlFlowAnalysis cfa(testProject.programLoader);
  cfa.functionCFA(airtimeHop);
  
  DataFlowAnalysis dfa(testProject.programLoader);
  dfa.functionDFA(airtimeHop);

  //outputDfgDot(std::cout, testProject.programLoader, airtimeHop);
  //EXPECT_FALSE(true);
}

TEST(DataFlowAnalysisTest, seAngleAxis) {
  filesystem::path test_path = filesystem::path(TEST_PATH) / "binaries";
  // mkw binaries. TODO: test on more convenient binaries
  vector<filesystem::path> files {test_path / "main.dol", test_path / "StaticR.rel"};

  ProjectCreationOptions options;
  options.projectName = "test_project";
  options.programLoaderType = persistence::LOADER_RVL;
  options.inputFiles = files;
  options.projectFile= "./test.ppc2cpp";
  Project testProject = Project::createProject(options);

  Function setAngleAxis(testProject.programLoader->getLocation("main.dol", ".text2", 0x8023a0a0-0x800072c0).value(), 0x8023a138-0x8023a0a0, "setAngleAxis");
  ControlFlowAnalysis cfa(testProject.programLoader);
  cfa.functionCFA(setAngleAxis);
  
  DataFlowAnalysis dfa(testProject.programLoader);
  dfa.functionDFA(setAngleAxis);

  //outputDfgDot(std::cout, testProject.programLoader, setAngleAxis);
  //EXPECT_FALSE(true);
}