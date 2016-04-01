#include "TimeUtils.h"
#include "Logging.h"
#include "ActiveWorker.h"

namespace colinli{
  namespace utility{
    namespace logging{
      using colinli::utility::time::LocalTime;
      //TODO
      class LoggerImpl
      {
          public:
              /** default constructor*/
              LoggerImpl();

          private:
              ActiveWorker bgworker_;
              LOG_LEVEL loglevel_;




      };


      /***************************************************/
      /************ LogMessage ***************************/

      //TODO
      LogMessage::LogMessage() {

      }

      void LogMessage::saveMessage(const char *msg_format, ...) {

      }

      std::ostringstream & LogMessage::messageStream() {
        return <#initializer#>;
      }

      LogMessage::~LogMessage() {

      }

      LogMessage::LogMessage(const std::string &file, const std::string &function, const std::string &line,
                             const std::string &levelname) {

      }

      /************ End of LogMessage *********************/
      /***************************************************/



      /*****************************************************/
      /************* Logging *******************************/

      void Logging::AddFileHandler(const std::string &filename) {

      }

      void Logging::AddStreamHandler(FILE *stream) {

      }

      void Logging::AddStreamHandler(std::basic_ostream &stream) {

      }

      void Logging::AddCustomHandler(Logging::Handler sink) {

      }

      void Logging::SetLevel(LOG_LEVEL level) {

      }

      std::string Logging::GetLevelName() {

      }

      Logging::Logging() {

      }

      /******************** End of Logging **************************/
      /**************************************************************/
    }
  }
}