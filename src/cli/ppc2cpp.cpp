
#include <cstring>
#include <fstream>

#include "ppc2cpp/model/Project.hpp"
#include "ppc2cpp/program_loader/ElfProgramLoader.hpp"
#include "ppc2cpp/program_loader/NinProgramLoader.hpp"
#include "ppc2cpp/analysis/PpcdisAnalysis.hpp"
#include "ppc2cpp/analysis/ProgramComparator.hpp"
#include "ppc2cpp/data_flow/DataFlowAnalysis.hpp"
#include "ppc2cpp/control_flow/ControlFlowAnalysis.hpp"
#include "ppc2cpp/data_flow/DfgVisualizer.hpp"

using namespace std;
using namespace ppc2cpp;

void print_create_usage(int exitcode) {
  cout << "Create a new ppc2cpp project from binaries\n"
          "usage: ppc2cpp create --output project_path [--name project_name]"
                " binary1 binary2...\n"
          "  options:\n"
          "    --output, -o     Created project file\n"
          "    --name, -n       Project name. Defaults to output filename stem"
                                                            " if not specified\n";
  exit(exitcode);
}

void cli_create(int argc, char** argv) {
  ProjectCreationOptions options;

  for (int i = 2; i < argc; i++) {
    if (strcmp("--out", argv[i]) == 0 || strcmp("-o", argv[i]) == 0) {
      if (i+1 >= argc) {
        print_create_usage(2);
      }
      options.projectFile = argv[++i];
    } else if (strcmp("--name", argv[i]) == 0 || strcmp("-n", argv[i]) == 0) {
      if (i+1 >= argc) {
        print_create_usage(3);
      }
      options.projectName = argv[++i];
    } else if (strcmp("--help", argv[i]) == 0 || strcmp("-h", argv[i]) == 0) {
      print_create_usage(0);  
    } else {
      if (!filesystem::is_regular_file(argv[i])) {
        cout << "Could not open file " << argv[i] << '\n';
        exit(4);
      }
      options.inputFiles.push_back(argv[i]);
    }
  }

  if (options.projectFile.empty()) {
    print_create_usage(5);
  }
  if (options.projectName.size() == 0) {
    options.projectName = options.projectFile.stem();
  }
  if (options.inputFiles.size() == 0) {
    print_create_usage(6);
  }

  // infer program loader type from binaries
  if (ElfProgramLoader::isElfProgram(options.inputFiles)) {
    options.programLoaderType = persistence::LOADER_ELF;
  } else if (NinProgramLoader::isRvlProgram(options.inputFiles)) {
    options.programLoaderType = persistence::LOADER_RVL;
  }

  Project::createProject(options);
  cout << "Project " << options.projectName << " created successfully at file " <<
    options.projectFile << '\n';
}

void print_importppcdis_usage(int exitcode) {
  cout << "Load analysis information from ppcdis to a project\n"
          "usage: ppc2cpp importppcdis --input project --symbols symbolMap"
                " [--ppcdis bin_yaml label_proto reloc_proto...]\n"
          "  options:\n"
          "    --input, -i     Project to import analysis to\n"
          "    --symbols, -s   The YAML symbol map file in ppcdis format\n"
          "    --ppcdis, -p    The three files to import. Should be in the order:"
                " binary yaml, label proto file and then relocs proto file. "
                "This flag can be repeated more than once\n";
  exit(exitcode);
}

void cli_import_ppcdis(int argc, char** argv) {
  filesystem::path projectProtoFile;
  string symbolMap;
  vector<PpcdisInfoFiles> ppcdisInfoFiles;

  for (int i = 2; i < argc; i++) {
    if (strcmp("--input", argv[i]) == 0 || strcmp("-i", argv[i]) == 0) {
      if (i+1 >= argc) {
        print_importppcdis_usage(7);
      }
      if (!filesystem::is_regular_file(argv[i+1])) {
        cout << "Could not open file " << argv[i+1] << '\n';
        exit(13);
      }
      projectProtoFile = argv[++i];
    } else if (strcmp("--ppcdis", argv[i]) == 0 || strcmp("-p", argv[i]) == 0) {
      if (i+3 >= argc) {
        print_importppcdis_usage(8);
      }
      for (int j = 1; j <= 3; j++) {
        if (!filesystem::is_regular_file(argv[i+j])) {
          cout << "Could not open file " << argv[i+j] << '\n';
          exit(13);
        }
      }
      ppcdisInfoFiles.push_back({argv[i+1], argv[i+2], argv[i+3]});
      i+=3;
    }  else if (strcmp("--symbols", argv[i]) == 0 || strcmp("-s", argv[i]) == 0) {
      if (i+1 >= argc) {
        print_importppcdis_usage(9);
      }
      if (!filesystem::is_regular_file(argv[i+1])) {
        cout << "Could not open file " << argv[i+1] << '\n';
        exit(13);
      }
      symbolMap = argv[++i];
    }else {
      print_importppcdis_usage(11);
    }
  }

  if (projectProtoFile.empty()) {
    print_importppcdis_usage(10);
  }
  if (symbolMap.size() == 0) {
    print_importppcdis_usage(12);
  }

  Project project = Project::openProject(projectProtoFile);
  loadAnalysisFromPpcdis(project.programLoader, ppcdisInfoFiles, symbolMap);
  project.saveProject();
}

void print_checkflow_usage(int exitcode) {
  cout << "Compare the equivalence of functions between two programs, by comparing their data flow graphs. Exit code 0 if all match,"
                                                        " other integer otherwise\n"
          "usage: ppc2cpp checkflow projectFile1 projectFile2 function1 function2 ...\n";
  exit(exitcode);
}

ProgramLocation getLocOrExit(const Project& project, uint32_t vma) {
  auto maybeLoc = project.programLoader->resolveVMA(vma);
  if (!maybeLoc.has_value()) {
    std::cout << "Could find VMA " << vma << " in program\n";
    exit(22);
  }
  return maybeLoc.value();
}

void cli_checkflow(int argc, char** argv) {
  if (argc < 5) {
    print_checkflow_usage(20);
  }

  filesystem::path projectFile1 = argv[2];
  filesystem::path projectFile2 = argv[3];

  if (!filesystem::is_regular_file(projectFile1)) {
    cout << "Could not open file " << projectFile1 << '\n';
    exit(21);
  }
  if (!filesystem::is_regular_file(projectFile2)) {
    cout << "Could not open file " << projectFile2 << '\n';
    exit(21);
  }

  Project project1 = Project::openProject(projectFile1);
  Project project2 = Project::openProject(projectFile2);

  ProgramComparator programComparator(project1.programLoader, project2.programLoader);
  for (int i = 4; i < argc; i++) {
    std::optional<Symbol> maybeSym1, maybeSym2;
    if (uint32_t vma = std::atol(argv[i])) {
      auto loc1 = getLocOrExit(project1, vma);
      auto loc2 = getLocOrExit(project1, vma);
      maybeSym1 = project1.programLoader->symtab.lookupByLocation(loc1);
      maybeSym2 = project2.programLoader->symtab.lookupByLocation(loc2);
    } else {
      maybeSym1 = project1.programLoader->symtab.lookupByName(argv[i]);
      maybeSym2 = project2.programLoader->symtab.lookupByName(argv[i]);
    }
    if (!maybeSym1.has_value()) {
      std::cout << "Could not find " << argv[i] << " in first project\n";
      exit(22);
    }
    if (!maybeSym2.has_value()) {
      std::cout << "Could not find " << argv[i] << " in second project\n";
      exit(22);
    }

    Function func1(maybeSym1.value());
    Function func2(maybeSym2.value());
    ControlFlowAnalysis cfa1(project1.programLoader);
    ControlFlowAnalysis cfa2(project2.programLoader);
    cfa1.functionCFA(func1);
    cfa2.functionCFA(func2);
    DataFlowAnalysis dfa1(project1.programLoader);
    DataFlowAnalysis dfa2(project2.programLoader);
    dfa1.functionDFA(func1);
    dfa2.functionDFA(func2);

    bool funcsEqual = programComparator.compareFunctionFlows(func1, func2);
    if (!funcsEqual) {
      std::cout << "Function " << argv[i] << " implementations are not equivalent\n";
      exit(23);
    }
  }
  exit(0);
}

void print_compare_usage(int exitcode) {
  cout << "Compare the equivalence of two programs. Exit code 0 if matches,"
                                                        " other integer otherwise\n"
          "usage: ppc2cpp compare projectFile1 projectFile2\n";
  exit(exitcode);
}

void cli_compare(int argc, char** argv) {
  if (argc != 4) {
    print_compare_usage(15);
  }

  filesystem::path projectFile1 = argv[2];
  filesystem::path projectFile2 = argv[3];

  if (!filesystem::is_regular_file(projectFile1)) {
    cout << "Could not open file " << projectFile1 << '\n';
    exit(16);
  }
  if (!filesystem::is_regular_file(projectFile2)) {
    cout << "Could not open file " << projectFile2 << '\n';
    exit(16);
  }

  Project project1 = Project::openProject(projectFile1);
  Project project2 = Project::openProject(projectFile2);

  ProgramComparator programComparator(project1.programLoader, project2.programLoader);
  if (programComparator.comparePrograms()) {
    exit(0);
  } else {
    exit(-1);
  }
}

void print_viz_usage(int exitcode) {
  cout << "Visualize symbols in a program\n"
          "usage: ppc2cpp importppcdis --input project --output output symbolName\n"
          "  options:\n"
          "    --input, -i     ppc2cpp program that contains the symbol\n"
          "    --output, -o    File to write visualization to\n";
  exit(exitcode);
}

void cli_viz(int argc, char** argv) {
  filesystem::path projectFile;
  filesystem::path vizFile;
  string symName;

  for (int i = 2; i < argc; i++) {
    if (strcmp("--out", argv[i]) == 0 || strcmp("-o", argv[i]) == 0) {
      if (i+1 >= argc) {
        print_viz_usage(2);
      }
      vizFile = argv[++i];
    } else if (strcmp("--input", argv[i]) == 0 || strcmp("-i", argv[i]) == 0) {
      if (i+1 >= argc) {
        print_viz_usage(3);
      }
      projectFile = argv[++i];
    } else if (strcmp("--help", argv[i]) == 0 || strcmp("-h", argv[i]) == 0) {
      print_viz_usage(0);  
    } else {
      symName = argv[i];
    }
  }

  if (projectFile.empty() || vizFile.empty() || symName.empty()) {
    print_viz_usage(8);
  }

  if (!filesystem::is_regular_file(projectFile)) {
    cout << "Could not open file " << projectFile << '\n';
    exit(16);
  }

  Project project = Project::openProject(projectFile);

  auto maybeSym = project.programLoader->symtab.lookupByName(symName);
  if (!maybeSym.has_value()) {
    std::cout << "Could not find " << symName << " in project\n";
    exit(22);
  }

  Function func(maybeSym.value());
  ControlFlowAnalysis cfa(project.programLoader);
  cfa.functionCFA(func);
  
  DataFlowAnalysis dfa(project.programLoader);
  dfa.functionDFA(func);

  ofstream dotfilestrm(vizFile, ios::out | ios::trunc);
  outputDfgDot(dotfilestrm, project.programLoader, func);
}

void print_verb_usage(int exitcode) {
  std::cout << "Available verbs:\n\n"
               "ppc2cpp create       Create a new pp2cpp project from a list of binaries\n"
               "ppc2cpp importppcdis Import analysis to existing project from ppcdis\n"
               "ppc2cpp checkflow    Compare function data flow graphs\n"
               "ppc2cpp viz          Visualize function control flow and data flow\n"
               "ppc2cpp compare      Compare two ppc2cpp programs\n";
  exit(exitcode);
}

int main(int argc, char** argv) {
  if (argc <= 1) {
    print_verb_usage(1);
  }
  if (strcmp("create", argv[1]) == 0) {
    cli_create(argc, argv);
  } else if (strcmp("importppcdis", argv[1]) == 0) {
    cli_import_ppcdis(argc, argv);
  } else if (strcmp("checkflow", argv[1]) == 0) {
    cli_checkflow(argc, argv);
  } else if (strcmp("compare", argv[1]) == 0) {
    cli_compare(argc, argv);
  } else if (strcmp("viz", argv[1]) == 0) {
    cli_viz(argc, argv);
  } else if (strcmp("--help", argv[1]) == 0 || strcmp("-h", argv[1]) == 0) {
    print_verb_usage(0);
  } else {
    print_verb_usage(1);
  }
}