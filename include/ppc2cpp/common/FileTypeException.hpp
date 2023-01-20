
#pragma once

#include <exception>
#include <string>

namespace ppc2cpp {
class FileTypeException : public std::exception {
public:
  FileTypeException(std::string filename) noexcept : filename(filename) {}
  const char * what () const throw () { return ("Unrecognised file " + filename).c_str(); }
  std::string getFilePath() const { return filename; }

private:
  std::string filename;
};
}