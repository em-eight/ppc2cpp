
#include <cstring>

#include "ppc2cpp/model/Project.hpp"

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
      filesystem::path inputFile(argv[i]);
      if (!filesystem::is_regular_file(inputFile)) {
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

void cli_import_ppcdis(int argc, char** argv) {

}

void cli_compare(int argc, char** argv) {

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