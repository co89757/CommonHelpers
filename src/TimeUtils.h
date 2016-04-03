#ifndef __COLINLI__TIME_UTILS__
#define __COLINLI__TIME_UTILS__ 
#include <chrono>
#include <ctime>
#include <string>

namespace colinli{
namespace utility{
namespace time{
  typedef std::chrono::steady_clock::time_point steady_time_point;
  typedef std::chrono::time_point<std::chrono::system_clock>  system_time_point;
  typedef std::chrono::milliseconds milliseconds;
  typedef std::chrono::microseconds microseconds;



  std::time_t Now();
  std::tm LocalTime(const std::time_t& time_point);
  std::string LocalTimeFormatted(const std::time_t& time_point, const char* time_format );
  std::string LocalTimeToString();

  }
 }
}



#endif