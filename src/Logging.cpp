
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdarg>
#include <algorithm>
#include "TimeUtils.h"
#include "Logging.h"
#include "Common.h"
#include "Path.h"

#define MAX_BUFFER_SIZE 1024

namespace colinli {
namespace utility {
namespace logging {
namespace internal {

int numerifyLogLevel(const std::string& sLogLevel) {
  const char* levels[] = { "INFO", "DEBUG", "WARNING", "ERROR" };

  for (int i = 0; i < 4; ++i) {
    if (sLogLevel.compare(levels[i]) == 0) return i + 1;
  }

  return -1;
}

bool tryOpenLogFile(const std::string& complete_file_with_path,
                    std::ofstream& outstream) {
  std::ios_base::openmode mode = std::ios_base::
      out;  // for clarity: it's really overkill since it's an ofstream
  mode |= std::ofstream::app;
  outstream.open(complete_file_with_path, mode);
  if (!outstream.is_open()) {
    std::ostringstream ss_error;
    ss_error << "FILE ERROR:  could not open log file:["
             << complete_file_with_path << "]";
    ss_error << "\n\t\t std::ios_base state = " << outstream.rdstate();
    std::cerr << ss_error.str().c_str() << std::endl << std::flush;
    outstream.close();
    return false;
  }
  std::ostringstream ss_entry;
  //  Day Month Date Time Year: is written as "%a %b %d %H:%M:%S %Y" and
  // formatted output as : Wed Sep 19 08:28:16 2012
  ss_entry << "\t\tg2log created log file at: " << time::LocalTimeToString()
           << "\n";
  outstream << ss_entry.str() << std::flush;
  outstream.fill('0');
  return true;
}

/**
 * create a pointer to ofstream of log file by given name
 */
std::unique_ptr<std::ofstream> createLogFile(
    const std::string& file_with_full_path) {
  std::unique_ptr<std::ofstream> out(new std::ofstream());
  std::ofstream& stream(*(out.get()));
  bool success_with_open_file = tryOpenLogFile(file_with_full_path, stream);
  if (!success_with_open_file) {
    out.release();  // nullptr contained ptr<file> signals error in creating the
                    // log file
  }
  return out;
}

void pushMessageToLog(const std::string& msg) {
  Logging::GetLogger().BroadcastMessage(msg);
}

}  // end of internal ns
}  // end of logging ns
}  // end of utility ns
}  // end of colinli ns

namespace colinli {
namespace utility {
namespace logging {

using colinli::utility::time::LocalTime;
using colinli::utility::time::LocalTimeToString;
using colinli::utility::time::LocalTimeFormatted;
using std::string;
using std::cout;

/***************************************************/
/************ LogMessage ***************************/

void LogMessage::saveMessage(const char* msg_format, ...) {

  // check if message level is lower than logger's level
  int loggerLogLevel = Logging::GetLogger().GetLevelAsNumber();
  if (nLevel_ < loggerLogLevel) {
    oss.str("");
    return;
  }

  char buffer[MAX_BUFFER_SIZE];
  va_list args;
  va_start(args, msg_format);
  int ncharacters = vsnprintf(buffer, sizeof(buffer), msg_format, args);
  va_end(args);
  if (ncharacters <= 0) {
    oss << "Failure to parse the message " << std::endl;
  } else if (ncharacters > MAX_BUFFER_SIZE) {
    buffer[MAX_BUFFER_SIZE - 1] = '\0';
    oss << buffer << " ... (the remaining is truncated.\n" << std::endl;
  } else {

    oss << buffer << std::endl;
  }

  internal::pushMessageToLog(oss.str());
  oss.str("");
}

std::ostringstream& LogMessage::messageStream() { return oss; }

LogMessage::~LogMessage() {
  // check if message level is lower than logger's level
  int loggerLogLevel = Logging::GetLogger().GetLevelAsNumber();
  if (nLevel_ >= loggerLogLevel && (!oss.str().empty())) {

    internal::pushMessageToLog(oss.str());
  }
}

LogMessage::LogMessage(const std::string& file, const std::string& function,
                       const int line, const std::string& levelname)
    : levelname_(levelname), file_(file), function_(function), line_(line) {
  nLevel_ = internal::numerifyLogLevel(levelname);
  // fallback on default log level on error (INFO level)
  if (nLevel_ < 0) {
    nLevel_ = ERROR;
  }

  auto name = utility::path::GetFilenameFromFullPath(file);
  oss.fill('0');
  oss << LocalTimeToString() << " [" << levelname << "] "
      << "@" << name << ":" << line << "\t";
}

/************ End of LogMessage *********************/

/***************************************************/

ALogMessageHandler::~ALogMessageHandler() {}

////////////////////// END OF ALogMessageHandler ////////////////////

//////////////////// FileLogger /////////////////////////////////

/**
 * FileLogger class that logs to given file
 */
class FileLogger : public ALogMessageHandler, private NonCopyable {
 public:
  /** default constructor*/
  FileLogger(const string& filepath);
  virtual void HandleMessage(const std::string& msg) override;
  ~FileLogger();

 private:
  void backgroundWriteToFile(const std::string& msg);
  std::ofstream& filestream();
  std::unique_ptr<std::ofstream> pFilestream_;
  std::unique_ptr<ActiveWorker> bg_worker_;
};

void FileLogger::HandleMessage(const std::string& msg) {
  // std::cout<<"FileLogger::HandleMessage ... \n";
  bg_worker_->send(std::bind(&FileLogger::backgroundWriteToFile, this, msg));

}

/** process the log message, the procedure is sent to bg_worker_ for async
 * processing. */
void FileLogger::backgroundWriteToFile(const std::string& msg) {
  std::ofstream& out(filestream());
  out << msg << std::flush;
}

FileLogger::FileLogger(const string& filepath)
    : pFilestream_(new std::ofstream()),
      bg_worker_(ActiveWorker::MakeActiveWorker()) {
  if (!internal::tryOpenLogFile(filepath, *pFilestream_)) {
    throw std::runtime_error("Cannot open log file");
  }
}
std::ofstream& FileLogger::filestream() { return *(pFilestream_.get()); }
FileLogger::~FileLogger() {
  bg_worker_.reset();
  // std::cout<<"FileLogger destructed ... \n";
}

////////////////END OF FILELOGGER ///////////////////////////

////////////// C-Filestream Logger /////////////////////////
class CStreamLogger : public ALogMessageHandler, private NonCopyable {
 public:
  /** default constructor*/
  CStreamLogger(FILE* file_handle);
  void HandleMessage(const std::string& msg) override;
  ~CStreamLogger();

 private:
  FILE* stream_ = NULL;
  std::unique_ptr<ActiveWorker> bg_worker_;
  void backgroundWriteToStream(const string& msg);

};

CStreamLogger::CStreamLogger(FILE* file_handle)
    : stream_(file_handle), bg_worker_(ActiveWorker::MakeActiveWorker()) {
  if (stream_ == NULL) {
    bg_worker_.reset();
    throw std::invalid_argument("Input stream is NULL.\n");
  }
}

void CStreamLogger::backgroundWriteToStream(const string& msg) {
  const char* content = msg.c_str();
  if (fputs(content, stream_) < 0) {
    throw std::runtime_error(
        "Error occurred on attempt to write to given C-stream.\n");
  }
  fflush(stream_);

}

void CStreamLogger::HandleMessage(const std::string& msg) {
  bg_worker_->send(
      std::bind(&CStreamLogger::backgroundWriteToStream, this, msg));

}

CStreamLogger::~CStreamLogger() { bg_worker_.reset(); }

/////////////// END of C-stream logger //////////////////

////////////////// LoggerImpl ////////////////////////
class LoggerImpl {
 public:
  /** default constructor*/
  LoggerImpl() {}
  void AddFileHandler(const std::string& filepath);
  void AddCStreamHandler(FILE* stream);

  void AddCustomHandler(ALogMessageHandler* handler);
  std::string GetLevelName() { return stringifyLevel(loglevel_); }
  void SetLevel(LOG_LEVEL level) { loglevel_ = level; }
  int GetLevel() { return loglevel_; }
  void BroadcastMessage(const std::string& msg);

  ~LoggerImpl() {}

 private:
  std::vector<ALogMessageHandler*> handlers_;
  LOG_LEVEL loglevel_ = INFO;
  std::string stringifyLevel(LOG_LEVEL level);
  std::unique_ptr<FileLogger> pFileLogger_;
  std::unique_ptr<CStreamLogger> pCStreamLogger_;
};

/*****************************************************/

void Logging::AddFileHandler(const std::string& filename) {
  if (pimpl) {
    pimpl->AddFileHandler(filename);
  }
}

void Logging::AddStreamHandler(FILE* stream) {
  if (pimpl) {
    pimpl->AddCStreamHandler(stream);
  }
}

void Logging::AddCustomHandler(ALogMessageHandler* sink) {
  if (pimpl) {
    pimpl->AddCustomHandler(sink);
  }
}

void Logging::SetLevel(LOG_LEVEL level) {
  if (pimpl) {
    pimpl->SetLevel(level);
  }
}

std::string Logging::GetLevelName() {
  if (pimpl) {
    return pimpl->GetLevelName();
  }

  return "";
}

int Logging::GetLevelAsNumber() {
  if (pimpl) {
    return pimpl->GetLevel();
  }
  return -1;
}
/**
 * private constructor
 */
Logging::Logging() : pimpl(new LoggerImpl()) {}

/** default destructor  */
Logging::~Logging() {}

void Logging::BroadcastMessage(const std::string& msg) {
  if (pimpl) pimpl->BroadcastMessage(msg);
}

/******************** End of Logging **************************/
/**************************************************************/

/////////////// LoggerImpl //////////////////////////////////

void LoggerImpl::AddFileHandler(const std::string& filepath) {
  pFileLogger_.reset(new FileLogger(filepath));
  handlers_.push_back(pFileLogger_.get());
}

void LoggerImpl::BroadcastMessage(const std::string& msg) {
    // std::cout<<"Logger:Broadcast msg...\n";
  std::for_each(handlers_.begin(), handlers_.end(),
                [&](ALogMessageHandler * worker) {
    worker->HandleMessage(msg);
  });
}
void LoggerImpl::AddCStreamHandler(FILE* stream) {
  pCStreamLogger_.reset(new CStreamLogger(stream));
  handlers_.push_back(pCStreamLogger_.get());
}

std::string LoggerImpl::stringifyLevel(LOG_LEVEL level) {
  switch (level) {
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

void LoggerImpl::AddCustomHandler(ALogMessageHandler* handler) {}

}  // end of logging ns
}  // end of utility ns
}  // end of colinli ns
