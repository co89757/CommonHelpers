#include <string>
#include <memory>
#include <sstream>

#ifndef ___PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__  __FUNCTION__
#endif


namespace colinli{
  namespace utility{
    namespace logging{

      typedef enum {
        INFO = 1,
        DEBUG =2,
        WARNING = 3,
        ERROR=4
      } LOG_LEVEL ;
      class LoggerImpl;

      class Logging
      {
        public:
          typedef std::function<void(const std::string& msg)> Handler;
          static Logging& GetLogger() {
            static Logging logger;
            return logger;

          }
          void AddFileHandler(const std::string & filename);
          void AddStreamHandler(FILE* stream);
          void AddStreamHandler(std::basic_ostream & stream);
          void AddCustomHandler(Handler sink);
          std::string GetLevelName();
          void SetLevel(LOG_LEVEL level);
        private:
          Logging(const Logging& ) = delete;
          Logging& operator=(const Logging& ) = delete;
          Logging();
          std::unique_ptr<LoggerImpl> pimpl;
      };

      class LogMessage
      {
          public:
              /** default constructor*/
              LogMessage();
              LogMessage(
                  const std::string& file,
                  const std::string& function,
                  const std::string& line,
                  const std::string& levelname
              );
              void saveMessage(const char* msg_format, ...);
              std::ostringstream& messageStream();
              ~LogMessage();
          private:
              std::ostringstream oss;
              std::string levelname_;
              std::string function_;
              std::string line_;
              std::string file_;
              std::string timestamp_;
              std::string message_;

      };

    }
  }
}

/** public macros */
#define LOG_INFO colinli::utility::logging::\
LogMessage(__FILE__, __PRETTY_FUNCTION__, __LINE__, "INFO" )

#define LOG_DEBUG colinli::utility::logging::\
LogMessage(__FILE__, __PRETTY_FUNCTION__, __LINE__, "DEBUG" )

#define LOG_WARNING colinli::utility::logging::\
LogMessage(__FILE__, __PRETTY_FUNCTION__, __LINE__, "WARNING" )

#define LOG_ERROR colinli::utility::logging::\
LogMessage(__FILE__, __PRETTY_FUNCTION__, __LINE__, "ERROR" )

#define LOG(level) LOG_##level.messageStream()
#define LOG_FMT(level, string_format, ...) LOG_##level.saveMessage(string_format, ##__VA_ARGS__)
