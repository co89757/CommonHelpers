//
// Created by colin on 3/31/16.
//
#include "Common.h"

namespace colinli{
  namespace utility{
    // check for filename validity -  filename should not be part of PATH
    bool file::isValidFilename(const std::string prefix_filename) {

      std::string illegal_characters("/,|<>:#$%{}()[]\'\"^!?+* ");
      size_t pos = prefix_filename.find_first_of(illegal_characters, 0);
      if (pos != std::string::npos) {
        std::cerr << "Illegal character [" << prefix_filename.at(pos) << "] in logname prefix: " << "[" << prefix_filename << "]" << std::endl;
        return false;
      } else if (prefix_filename.empty()) {
        std::cerr << "Empty filename prefix is not allowed" << std::endl;
        return false;
      }

      return true;
    }

    std::string file::pathSanityFix(std::string path, std::string file_name) {
     // Unify the delimeters,. maybe sketchy solution but it seems to work
     // on at least win7 + ubuntu. All bets are off for older windows
     std::replace(path.begin(), path.end(), '\\', '/');

     // clean up in case of multiples
     auto contains_end = [&](std::string & in) -> bool {
        size_t size = in.size();
        if (!size) return false;
        char end = in[size - 1];
        return (end == '/' || end == ' ');
     };

     while (contains_end(path)) { path.erase(path.size() - 1); }
     if (!path.empty()) {
        path.insert(path.end(), '/');
     }
     path.insert(path.size(), file_name);
     return path;
    }






}//end of utility ns
}// end of colinli ns 
