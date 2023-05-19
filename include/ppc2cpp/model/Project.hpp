
#include "ppc2cpp/program_loader/ProgramLoader.hpp"

#define PROJECT_PROTO_CURRENT_VERSION 1

namespace ppc2cpp {
/**
 * A POD struct containing configuration passed to createProject when a user creates a new project
*/
struct ProjectCreationOptions {
  // path of input files (binaries) for program loader
  std::vector<std::filesystem::path> inputFiles;
  // output path of the created project persistence file
  std::filesystem::path projectFile;
  std::string projectName;

  persistence::ProgramLoaderType programLoaderType;
};


/**
 * Serves as the main context for a program decompilation. Contains references to the program loader, symbol tables, analysis artifacts etc
*/
class Project {
public:
  static Project createProject(const ProjectCreationOptions& options);
  void openProject(const std::filesystem::path& projectFile);
  void saveProject();
  void saveProjectAs(const std::filesystem::path& projectFile);

  std::string name;
  ProgramLoaderPtr programLoader;

private:
  std::filesystem::path projectFile;
};
}