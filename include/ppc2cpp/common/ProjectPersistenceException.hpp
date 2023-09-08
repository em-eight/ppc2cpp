
#pragma once

#include <exception>
#include <string>

namespace ppc2cpp {
class ProjectPersistenceException : public std::exception {
public:
  ProjectPersistenceException(std::string what) noexcept : what_str(what.c_str()) {}
  const char * what () const throw () { return what_str; }

private:
  const char* what_str;
};
}