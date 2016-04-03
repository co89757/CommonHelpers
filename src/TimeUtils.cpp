#include "TimeUtils.h"

namespace colinli{
    namespace utility{
        namespace time{
          static const char* DEFAULT_TIME_FMT = "%F %T";

class Constants
{
public:
  constexpr static const char* DefaultDateTimeFormat = "%F %T";
private:
  Constants(){}
};



namespace {
#define MAX_BUFFER_SIZE 1024

std::string putTime(struct tm* tmb, const char* time_format){
    char buffer[MAX_BUFFER_SIZE];
    int success = strftime(buffer, MAX_BUFFER_SIZE, time_format, tmb);
    if(success == 0){
        //buffer overflow, only return time format as a hack for now 
        return time_format;
    }
    return buffer;
}


}//end of file scope utils 


std::time_t Now()
{
    auto now = std::chrono::system_clock::now();
    return std::chrono::system_clock::to_time_t(now);
}

std::tm LocalTime(const std::time_t& time_point)
{
    struct tm tm_snapshot;
#if !(defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
  localtime_r(&time_point, &tm_snapshot); // POSIX
#else
  localtime_s(&tm_snapshot, &time_point); // windsows
#endif
  return tm_snapshot;
}

std::string LocalTimeFormatted(const std::time_t& time_point, const char* time_format )
{
    std::tm tm_snapshot = LocalTime(time_point);
    return putTime(&tm_snapshot, time_format );
}

std::string LocalTimeToString()
{
    return LocalTimeFormatted(Now(), DEFAULT_TIME_FMT);
}



}
}
}

