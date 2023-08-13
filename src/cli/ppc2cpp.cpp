
#include <cstring>

#include "ppc2cpp/model/Project.hpp"
#include "ppc2cpp/analysis/PpcdisAnalysis.hpp"
#include "ppc2cpp/analysis/ProgramComparator.hpp"

using namespace std;
using namespace ppc2cpp;

void print_create_usage(int exitcode) {
  // TODO
  exit(exitcode);
}

void cli_create(int argc, char** argv) {
  ProjectCreationOptions options;

  for (int i = 2; i < argc; i++) {
    if (strcmp("--out", argv[i]) == 0 || strcmp("-o", argv[i]) == 0) {
      if (i+1 >= argc) {
        print_create_usage(2);
      }
      options.projectFile = argv[i+1];
    } else if (strcmp("--name", argv[i]) == 0 || strcmp("-n", argv[i]) == 0) {
      if (i+1 >= argc) {
        print_create_usage(3);
      }
      options.projectName = argv[i+1];
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

  if (options.projectFile.empty() == 0) {
    print_create_usage(5);
  }
  if (options.projectName.size() == 0) {
    options.projectName = options.projectFile.stem();
  }
  if (options.inputFiles.size() == 0) {
    print_create_usage(6);
  }

  Project::createProject(options);
  cout << "Project " << options.projectName << " created successfully at file " <<
    options.projectFile << '\n';
}

void print_importppcdis_usage(int exitcode) {
  // TODO
  exit(exitcode);
}

void cli_import_ppcdis(int argc, char** argv) {
  filesystem::path projectProtoFile;
  string symbolMap;
  vector<PpcdisInfoFiles> ppcdisInfoFiles;

  for (int i = 2; i < argc; i++) {
    if (strcmp("--in", argv[i]) == 0 || strcmp("-i", argv[i]) == 0) {
      if (i+1 >= argc) {
        print_importppcdis_usage(7);
      }
      if (!filesystem::is_regular_file(argv[i+1])) {
        cout << "Could not open file " << argv[i+1] << '\n';
        exit(13);
      }
      projectProtoFile = argv[i+1];
    } else if (strcmp("--ppcdis", argv[i]) == 0 || strcmp("-p", argv[i]) == 0) {
      if (i+3 >= argc) {
        print_importppcdis_usage(8);
      }
      for (int j = 1; j <= 4; j++) {
        if (!filesystem::is_regular_file(argv[i+j])) {
          cout << "Could not open file " << argv[i+j] << '\n';
          exit(13);
        }
      }
      ppcdisInfoFiles.push_back({argv[i+1], argv[i+2], argv[i+3]});
    }  else if (strcmp("--symbols", argv[i]) == 0 || strcmp("-s", argv[i]) == 0) {
      if (i+1 >= argc) {
        print_importppcdis_usage(9);
      }
      if (!filesystem::is_regular_file(argv[i+1])) {
        cout << "Could not open file " << argv[i+1] << '\n';
        exit(13);
      }
      symbolMap = argv[i+1];
    }else {
      print_importppcdis_usage(11);
    }
  }

  if (projectProtoFile.empty()) {
    print_create_usage(10);
  }
  if (symbolMap.size() == 0) {
    print_create_usage(12);
  }

  Project project = Project::openProject(projectProtoFile);
  loadAnalysisFromPpcdis(project.programLoader, ppcdisInfoFiles, symbolMap);
  project.saveProject();
}

void print_compare_usage(int exitcode) {
  // TODO
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

void print_verb_usage(int exitcode) {
  // TODO
  exit(exitcode);
}

int main(int argc, char** argv) {
  if (strcmp("create", argv[1]) == 0) {
    cli_create(argc, argv);
  } else if (strcmp("importppcdis", argv[1]) == 0) {
    cli_import_ppcdis(argc, argv);
  } else if (strcmp("compare", argv[1]) == 0) {
    cli_compare(argc, argv);
  } else if (strcmp("--help", argv[1]) == 0 || strcmp("-h", argv[1]) == 0) {
    print_verb_usage(0);
  } else {
    print_verb_usage(1);
  }
}