//
//  logging.h
//  HiveApp
//
//  Created by levilin on 8/17/20.
//  Copyright © 2020 levilin. All rights reserved.
//

#ifndef logging_hpp
#define logging_hpp

#include <assert.h>
#include <string>
#include <sstream>
#include "build/base_export.h"


namespace hive {

typedef int LogSeverity;

const LogSeverity LOG_INFO = 0;
const LogSeverity LOG_WARNING = 1;
const LogSeverity LOG_ERROR = 2;
const LogSeverity LOG_FATAL = 3;
const LogSeverity LOG_NUM_SEVERITIES = 4;

typedef bool (*LogMessageHandlerFunction)(int severity,
    const char* file, int line, size_t message_start, const std::string& str);

BASE_EXPORT void SetLogMessageHandler(LogMessageHandlerFunction handler);

BASE_EXPORT void SetLogPrefix(const char* prefix);

// INFO
#define COMPACT_LOG_INFO \
::hive::LogMessage(__FILE__, __LINE__, ::hive::LOG_INFO)

// WARNING
#define COMPACT_LOG_WARNING \
::hive::LogMessage(__FILE__, __LINE__, ::hive::LOG_WARNING)

// ERROR
#define COMPACT_LOG_ERROR \
::hive::LogMessage(__FILE__, __LINE__, ::hive::LOG_ERROR)

// FATAL
#define COMPACT_LOG_FATAL \
::hive::LogMessage(__FILE__, __LINE__, ::hive::LOG_FATAL)


#define LOG_STREAM(severity) COMPACT_LOG_ ## severity.stream()

#define LAZY_STREAM(stream, condition)                                  \
!(condition) ? (void) 0 : ::hive::LogMessageVoidify() & (stream)

#define LOG(severity) LAZY_STREAM(LOG_STREAM(severity), true)

#if defined(NDEBUG)
    
    #define DCHECK_IS_ON() 0

#else

    #define DCHECK_IS_ON() 1

#endif


#if DCHECK_IS_ON()

    #define DCHECK(condition)  assert(condition)

#else

    #define DCHECK(condition)

#endif /* DCHECK_IS_ON */

class BASE_EXPORT LogMessage {
public:
    // Used for LOG(severity).
    LogMessage(const char* file, int line, LogSeverity severity);
    
    // Used for CHECK().  Implied severity = LOG_FATAL.
    LogMessage(const char* file, int line, const char* condition);
    
    // Used for CHECK_EQ(), etc. Takes ownership of the given string.
    // Implied severity = LOG_FATAL.
    LogMessage(const char* file, int line, std::string* result);
    
    // Used for DCHECK_EQ(), etc. Takes ownership of the given string.
    LogMessage(const char* file, int line, LogSeverity severity,
               std::string* result);
    
    ~LogMessage();
    
    std::ostream& stream() { return stream_; }
    
    LogSeverity severity() { return severity_; }
    std::string str() { return stream_.str(); }
    
private:
    void Init(const char* file, int line);
    
    LogSeverity severity_;
    std::ostringstream stream_;
    size_t message_start_;  // Offset of the start of the message (past prefix
    // info).
    // The file and line information passed in to the constructor.
    const char* file_;
    const int line_;
    
    LogMessage(const LogMessage&) = delete;
    LogMessage& operator=(const LogMessage&) = delete;
};

class LogMessageVoidify {
 public:
  LogMessageVoidify() = default;
  // This has to be an operator with a precedence lower than << but
  // higher than ?:
  void operator&(std::ostream&) { }
};

}; // 

#endif /* logging_hpp */
