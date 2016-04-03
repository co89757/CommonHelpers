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

} // end of colinli ns 

#endif  