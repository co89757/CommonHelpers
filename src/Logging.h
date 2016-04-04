#include <string>
#include <memory>
#include <sstream>
#include <memory>
#include "ActiveWorker.h"

#ifndef ___PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCTION__
#endif

namespace colinli {
namespace utility {
namespace logging {

typedef enum {
  INFO = 1,
  DEBUG = 2,
  WARNING = 3,
  ERROR = 4
} LOG_LEVEL;
class LoggerImpl;

/**
 * base abstract class that encapuslates the behavior of an async log worker of
 * given sink method
 */
class ALogMessageHandler {
 public:
  virtual void HandleMessage(const std::string& msg) = 0;
  /** send the HandleMessage task down to background worker */
  // void SendLogTaskToBackground(const std::string& msg);
  virtual ~ALogMessageHandler();
  // protected:

  //   std::unique_ptr<ActiveWorker> bg_worker_;

};

class Logging {
 public:
  Logging(const Logging&) = delete;
  Logging& operator=(const Logging&) = delete;
  /** The only method to get a singleton logging instance */
  static Logging& GetLogger() {
    static Logging logger;
    return logger;

  }
  void AddFileHandler(const std::string& filename);
  void AddStreamHandler(FILE* stream);

  void AddCustomHandler(ALogMessageHandler* sink);
  void BroadcastMessage(const std::string& msg);
  std::string GetLevelName();
  int GetLevelAsNumber();

  void SetLevel(LOG_LEVEL level);
  ~Logging();

 private:

  Logging();
  std::unique_ptr<LoggerImpl> pimpl;
};

/**
 * LogMessage class is responsible for reformatting the log message
 */
class LogMessage {
 public:

  LogMessage(const std::string& file, const std::string& function,
             const int line, const std::string& levelname);
  void saveMessage(const char* msg_format, ...);

  std::ostringstream& messageStream();
  ~LogMessage();

 private:
  int nLevel_ = 1;
  std::ostringstream oss;
  std::string levelname_;
  std::string file_;
  std::string function_;
  int line_;
  std::string datetime_format_;
  std::string message_;

};

}  // end of logging ns
}  // end of utility ns
}  // end of colinli ns

/** public macros */

#define LOG_SET_LEVEL_DEBUG                                 \
  colinli::utility::logging::Logging::GetLogger().SetLevel( \
      colinli::utility::logging::DEBUG)

#define LOG_SET_LEVEL_INFO                                  \
  colinli::utility::logging::Logging::GetLogger().SetLevel( \
      colinli::utility::logging::INFO)

#define LOG_SET_LEVEL_WARNING                               \
  colinli::utility::logging::Logging::GetLogger().SetLevel( \
      colinli::utility::logging::WARNING)

#define LOG_SET_LEVEL_ERROR                                 \
  colinli::utility::logging::Logging::GetLogger().SetLevel( \
      colinli::utility::logging::ERROR)

#define LOG_SET_LEVEL(level) LOG_SET_LEVEL_##level

#define LOG_INFO                                                       \
  colinli::utility::logging::LogMessage(__FILE__, __PRETTY_FUNCTION__, \
                                        __LINE__, "INFO")

#define LOG_DEBUG                                                      \
  colinli::utility::logging::LogMessage(__FILE__, __PRETTY_FUNCTION__, \
                                        __LINE__, "DEBUG")

#define LOG_WARNING                                                    \
  colinli::utility::logging::LogMessage(__FILE__, __PRETTY_FUNCTION__, \
                                        __LINE__, "WARNING")

#define LOG_ERROR                                                      \
  colinli::utility::logging::LogMessage(__FILE__, __PRETTY_FUNCTION__, \
                                        __LINE__, "ERROR")

#define LOG(level) LOG_##level.messageStream()
#define LOGF(level, string_format, ...) \
  LOG_##level.saveMessage(string_format, ##__VA_ARGS__)
