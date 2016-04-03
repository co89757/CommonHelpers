#ifndef COLINLI_COMMON_H_
#define COLINLI_COMMON_H_ 

#include <string>
namespace colinli{

  class NonCopyable{
  public:
    NonCopyable() { }

  private:
    NonCopyable(const NonCopyable& ) = delete;
    NonCopyable& operator=(const NonCopyable& ) = delete;
  };


  namespace utility{
    namespace file{
      /**
       * Check validity of filename, filename should not include illegal 
       * chracters such as % / or // 
       * @param  prefix_filename [filename]
       * @return                 true: filename is valid false: invalid filename
       */
      bool isValidFilename(const std::string prefix_filename);
      /**
       * Fix up file full path by removing extraneous characters
       * @param  path      base directory of file
       * @param  file_name filename
       * @return           fixed-up edition of full path to file 
       */
      std::string pathSanityFix(std::string path, std::string file_name);
    }
  }
}

#endif  