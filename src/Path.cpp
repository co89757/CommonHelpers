#include <iostream>
#include <algorithm>
#include "Path.h"

namespace colinli {
namespace utility {
namespace path {
using std::string;
/**
 * get the filename from the full path to the file
 * @param  filepath the full path to the file, e.g. full/path/to/file.c
 * @return          the file name, e.g. file.c from the above example
 */
std::string GetFilenameFromFullPath(const string& filepath) {
  string sep = "/";
#if defined(__WIN32__) || defined(_WIN32)
  sep += "\\";
#endif

  size_t pos = filepath.rfind(sep);
  if (pos != string::npos) {
    return filepath.substr(pos + 1, filepath.length() - pos);
  }
  // else, no sep char found
  if (IsValidFilename(filepath)) {
    return filepath;
  } else {
    return "";
  }

}

// check for filename validity -  filename should not be part of PATH
bool IsValidFilename(const std::string prefix_filename) {

  std::string illegal_characters("/,|<>:#$%{}()[]\'\"^!?+* ");
  size_t pos = prefix_filename.find_first_of(illegal_characters, 0);
  if (pos != std::string::npos) {
    std::cerr << "Illegal character [" << prefix_filename.at(pos)
              << "] in logname prefix: "
              << "[" << prefix_filename << "]" << std::endl;
    return false;
  } else if (prefix_filename.empty()) {
    std::cerr << "Empty filename prefix is not allowed" << std::endl;
    return false;
  }

  return true;
}

std::string PathSanityFix(std::string path, std::string file_name) {
  // Unify the delimeters,. maybe sketchy solution but it seems to work
  // on at least win7 + ubuntu. All bets are off for older windows
  std::replace(path.begin(), path.end(), '\\', '/');

  // clean up in case of multiples
  auto contains_end = [&](std::string & in)->bool {
    size_t size = in.size();
    if (!size) return false;
    char end = in[size - 1];
    return (end == '/' || end == ' ');
  }
  ;

  while (contains_end(path)) {
    path.erase(path.size() - 1);
  }
  if (!path.empty()) {
    path.insert(path.end(), '/');
  }
  path.insert(path.size(), file_name);
  return path;
}

}  // end of path ns
}  // end of utility ns
}  // end of colinli ns