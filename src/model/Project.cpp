
#include <fstream>
#include <iostream>

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

  programLoader.set_formatversion(PROGRAM_LOADER_PROTO_CURRENT_VERSION);
  programLoader.set_loadertype(options.programLoaderType);
  for (const auto& inputFile : options.inputFiles) {
    auto* binaryProto = programLoader.add_binaries();
    binaryProto->set_filename(inputFile.filename());

    std::ifstream fstream(inputFile, std::ios::binary | std::ios::in);
    if (!fstream.is_open()) throw std::runtime_error("Input file " + inputFile.string() + " was not found");
    std::stringstream sstream;
    sstream << fstream.rdbuf();
    binaryProto->set_data(sstream.str());
  }
  *projectFile.mutable_program_loader() = programLoader;

  {
    std::fstream output(options.projectFile.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);
    if (!projectFile.SerializeToOstream(&output)) {
      throw new ProjectPersistenceException("Failed to write project file");
    }
  }

  Project project;
  project.openProject(options.projectFile);
  return project;
}

void Project::openProject(const std::filesystem::path& projectFile) {
  persistence::Project projectProto;

  {
    std::ifstream fstream(projectFile.c_str(), std::ios::binary | std::ios::in);
    if (!projectProto.ParseFromIstream(&fstream)) {
      throw new ProjectPersistenceException("Failed to read project file");
    }
  }

  this->programLoader = ProgramLoader::fromProto(&projectProto.program_loader());
  this->projectFile = projectFile;
  this->name = projectProto.projectname();
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