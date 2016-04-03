#include "TimeUtils.h"
#include "Logging.h"
#include "ActiveWorker.h"
#include "Common.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <algorithm>
#define MAX_BUFFER_SIZE 1024

namespace colinli{
  namespace utility{
    namespace logging{
      namespace internal {
        bool TryOpenLogFile(const std::string& complete_file_with_path, std::ofstream& outstream) {
          std::ios_base::openmode mode = std::ios_base::out; // for clarity: it's really overkill since it's an ofstream
          mode |= std::ofstream::app;
          outstream.open(complete_file_with_path, mode);
          if (!outstream.is_open()) {
            std::ostringstream ss_error;
            ss_error << "FILE ERROR:  could not open log file:[" << complete_file_with_path << "]";
            ss_error << "\n\t\t std::ios_base state = " << outstream.rdstate();
            std::cerr << ss_error.str().c_str() << std::endl << std::flush;
            outstream.close();
            return false;
          }
          std::ostringstream ss_entry;
          //  Day Month Date Time Year: is written as "%a %b %d %H:%M:%S %Y" and formatted output as : Wed Sep 19 08:28:16 2012
          ss_entry << "\t\tg2log created log file at: " << time::LocalTimeToString() << "\n";
          outstream << ss_entry.str() << std::flush;
          outstream.fill('0');
          return true;
        }

        void pushMessageToLog(const std::string& msg)
        {
          Logging::GetLogger().BroadcastMessage(msg);
        }

      }
    }
  }
}


namespace colinli{
  namespace utility{
    namespace logging{
      using colinli::utility::time::LocalTime;
      using colinli::utility::time::LocalTimeToString;
      using std::string ;





      class FileLogger: public ALogMessageHandler, private NonCopyable
      {
          public:
              /** default constructor*/
              FileLogger(const string& filepath);
              virtual void HandleMessage(const std::string &msg);

              ~FileLogger();
          private:
              void backgroundWriteToFile(const std::string& msg);
              std::ofstream& filestream();
              std::unique_ptr<std::ofstream> pFilestream_;
              std::unique_ptr<ActiveWorker> bg_worker_;
      };






      //TODO
      class LoggerImpl
      {
          public:
              /** default constructor*/
              LoggerImpl():pFileLogger_(nullptr) {

              }
              void AddFileHandler(const std::string& filepath );
              void AddCStreamHandler(FILE *stream);
              void AddStreamHandler(std::basic_ostream<char>& stream);
              void AddCustomHandler(ALogMessageHandler *handler);
              std::string GetLevelName() {
                return stringifyLevel(loglevel_);
              }
              void SetLevel(LOG_LEVEL level) {
                loglevel_ = level;
              }
              int GetLevel(){
                return loglevel_ ;
              }
              void BroadcastMessage(const std::string& msg );
              ~LoggerImpl(){}
          private:
              std::vector< ALogMessageHandler* > handlers_;
              LOG_LEVEL loglevel_ = INFO;
              std::string stringifyLevel(LOG_LEVEL level);
              std::unique_ptr<FileLogger> pFileLogger_;
      };


      /***************************************************/
      /************ LogMessage ***************************/

      //TODO
      LogMessage::LogMessage() {

      }

      void LogMessage::saveMessage(const char *msg_format, ...) {
        char buffer[MAX_BUFFER_SIZE];
        va_list args;
          va_start(args, msg_format);
          int ncharacters = vsnprintf(buffer, sizeof(buffer), msg_format, args);
          va_end(args);
          if(ncharacters <= 0){
            oss << "Failure to parse the message "<<std::endl;
          }else if(ncharacters > MAX_BUFFER_SIZE){
            buffer[MAX_BUFFER_SIZE-1] = '\0';
            oss << buffer <<" ... (the remaining is truncated.\n"<<std::endl;
          }else{
            buffer[ncharacters] = '\0';
            oss << buffer;
          }

        internal::pushMessageToLog(oss.str());
        oss.str("");
      }

      std::ostringstream & LogMessage::messageStream() {
        return oss;
      }

      LogMessage::~LogMessage() {
        if(!oss.str().empty())
            internal::pushMessageToLog(oss.str());
      }

      LogMessage::LogMessage(const std::string &file, const std::string &function, const int line,
                             const std::string &levelname) {
            oss.fill('0');
            oss << LocalTimeToString()<<" ["<<levelname<<"] "<<"@"<<file<<":"<<line<<":";
      }

      /************ End of LogMessage *********************/
      /***************************************************/



      /*****************************************************/
      /************* Logging *******************************/

      void Logging::AddFileHandler(const std::string &filename) {
            pimpl->AddFileHandler(filename);
      }

      void Logging::AddStreamHandler(FILE *stream) {
        pimpl->AddCStreamHandler(stream);
      }

      void Logging::AddStreamHandler(std::basic_ostream<char> &stream) {
        pimpl->AddStreamHandler(stream);
      }

      void Logging::AddCustomHandler(ALogMessageHandler *sink) {
        pimpl->AddCustomHandler(sink);
      }

      void Logging::SetLevel(LOG_LEVEL level) {
        pimpl->SetLevel(level);
      }

      std::string Logging::GetLevelName() {
        return pimpl->GetLevelName();
      }

      /**
       * private constructor 
       */
      Logging::Logging():pimpl(new LoggerImpl()) {
      }

      /** default destructor  */
      Logging::~Logging(){
        //destructor 
      }

      void Logging::BroadcastMessage(const std::string &msg) {
          pimpl->BroadcastMessage(msg);
      }

      /******************** End of Logging **************************/
      /**************************************************************/

      //////////// FILE LOGGER //////////////////////////////////////
      void FileLogger::HandleMessage(const std::string &msg) {
        std::cout<<"FileLogger::HandleMessage ... \n";
        bg_worker_->send(std::bind(&FileLogger::backgroundWriteToFile,this, msg) );
      }

      FileLogger::FileLogger(const string &filepath):
          pFilestream_(new std::ofstream()),
          bg_worker_(ActiveWorker::MakeActiveWorker())

      {
        if(! internal::TryOpenLogFile(filepath, *pFilestream_)){
          throw std::runtime_error("Cannot open log file");
        }
      }
      std::ofstream& FileLogger::filestream()
      {
        return *(pFilestream_.get());
      }
      FileLogger::~FileLogger()
      {
        bg_worker_.reset();
        std::cout<<"FileLogger destructed ... \n";
      }

      void FileLogger::backgroundWriteToFile(const std::string& msg)
      {
        std::ofstream& out(filestream());
        out << msg << std::flush;
      }

      ////////////////END OF FILELOGGER ///////////////////////////

      /////////////// LoggerImpl //////////////////////////////////

      void LoggerImpl::AddFileHandler(const std::string &filepath) {
        pFileLogger_.reset(new FileLogger(filepath));
        handlers_.push_back( pFileLogger_.get() );
      }



      void LoggerImpl::BroadcastMessage(const std::string &msg) {
        std::cout<<"Logger:Broadcast msg...\n";
        pFileLogger_->HandleMessage(msg);
      }
      void LoggerImpl::AddCStreamHandler(FILE *stream) {

      }

      void LoggerImpl::AddStreamHandler(std::basic_ostream<char> &stream) {

      }

      std::string LoggerImpl::stringifyLevel(LOG_LEVEL level)
      {
          switch (level){
            case INFO:
              return "INFO";
            case DEBUG:
              return "DEBUG";
            case WARNING:
              return "WARNING";
            case ERROR:
              return "ERROR";
            default:
              return "UNKNOWN";
          }
      }

      void LoggerImpl::AddCustomHandler(ALogMessageHandler *handler) {

      }
    }
  }
}