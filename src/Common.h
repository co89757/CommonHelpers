#ifndef COLINLI_COMMON_H_
#define COLINLI_COMMON_H_ 

#include <string>




namespace colinli{

    class Constants
    {
    public:
        static constexpr const char* DEFAULT_TIME_FORMAT = "%F %T";
        static constexpr int MAX_BUFFER_SIZE = 1024;

    };



      class NonCopyable{
      public:
        NonCopyable() { }

      private:
        NonCopyable(const NonCopyable& ) = delete;
        NonCopyable& operator=(const NonCopyable& ) = delete;
      };

} // end of colinli ns 

#endif  