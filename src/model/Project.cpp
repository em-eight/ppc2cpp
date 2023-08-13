
#include <fstream>
#include <iostream>
#include <sstream>

#include "project.pb.h"

#include "ppc2cpp/common/ProjectPersistenceException.hpp"
#include "ppc2cpp/model/Project.hpp"

namespace ppc2cpp {
Project Project::createProject(const ProjectCreationOptions& options) {
  persistence::Project projectFile;
  persistence::ProgramLoader programLoader;

  projectFile.set_formatversion(PROJECT_PROTO_CURRENT_VERSION);
  projectFile.set_projectname(options.projectName);
  projectFile.set_projectversion(1);

  ProgramLoaderPtr pLoader = ProgramLoader::loadProgram(PROGRAM_LOADER_PROTO_CURRENT_VERSION, options.programLoaderType, options.inputFiles);
  pLoader->toProto(projectFile.mutable_program_loader());

  {
    std::fstream output(options.projectFile.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (!projectFile.SerializeToOstream(&output)) {
      throw new ProjectPersistenceException("Failed to write project file");
    }
  }

  return Project::openProject(options.projectFile);
}

Project Project::openProject(const std::filesystem::path& projectFile) {
  Project project;
  persistence::Project projectProto;

  {
    std::ifstream fstream(projectFile.c_str(), std::ios::binary | std::ios::in);
    if (!projectProto.ParseFromIstream(&fstream)) {
      throw new ProjectPersistenceException("Failed to read project file");
    }
  }

  project.programLoader = ProgramLoader::fromProto(&projectProto.program_loader());
  project.projectFile = projectFile;
  project.name = projectProto.projectname();

  return project;
}

void Project::saveProject() {
  this->saveProjectAs(this->projectFile);
}

void Project::saveProjectAs(const std::filesystem::path& projectFile) {
  persistence::Project projectFileProto;
  persistence::ProgramLoader programLoader;

  projectFileProto.set_formatversion(PROJECT_PROTO_CURRENT_VERSION);
  projectFileProto.set_projectname(this->name);
  projectFileProto.set_projectversion(1);

  this->programLoader->toProto(&programLoader);
  *projectFileProto.mutable_program_loader() = programLoader;

  {
    std::fstream output(projectFile.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (!projectFileProto.SerializeToOstream(&output)) {
      throw new ProjectPersistenceException("Failed to write project file");
    }
  }
}
}