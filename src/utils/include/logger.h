/* How to use.
 *      1. Create global logger in your file.
 *              -> CREATE_LOGGERPTR_GLOBAL(logger_ptr, "Your name for logger", "Path to your file or file name");
 *      2. Add autotrace to the start of your function.
 *              -> LOG_AUTO_TRACE(logger_ptr, "Your function name");
 *      3. Use one of the proposed log level.
 *              -> LOG_INFO(logger_ptr, "Your log message.", std::string, int, ...);
 *              -> LOG_DEBUG(logger_ptr, "Your log message.", std::string, int, ...);
 *              -> LOG_ERROR(logger_ptr, "Your log message.", std::string, int, ...);
 *
 * Note: Logs save to -> log.txt file in your binary directory.
*/

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <memory>
#include <vector>
#include <sstream>
#include <iostream>
#include <QString>

class Logger;

using LoggerPtr = std::shared_ptr<Logger>;

enum LOG_LEVEL {
   DEBUG = 0,
   INFO = 1,
   ERROR = 2
};

#define CREATE_LOGGERPTR_GLOBAL(logger_ptr, logger_name, file_name_or_path) \
    namespace {                                                             \
    LoggerPtr logger_ptr = std::make_shared<Logger>(logger_name, file_name_or_path); \
    }

#define LOG_DEBUG(logger_ptr, args ... ) \
   logger_ptr->prepare_messag_from_user(LOG_LEVEL::DEBUG, args) \

#define LOG_INFO(logger_ptr, args ... ) \
   logger_ptr->prepare_messag_from_user(LOG_LEVEL::INFO, args) \

#define LOG_ERROR(logger_ptr, args ... ) \
   logger_ptr->prepare_messag_from_user(LOG_LEVEL::ERROR, args) \

#define LOG_AUTO_TRACE(logger_ptr, function_name) \
    AutoTrace autoTrace(logger_ptr, function_name) \

class Logger {
public:
    Logger(const char *function_name, const char *file_name_or_path);
    ~Logger();

    template<typename ... Args>
    void prepare_messag_from_user(const LOG_LEVEL log_level, Args &&... args)
    {
        std::ostringstream strm;
        using dummy = char[];
        (void)dummy{ (strm << std::forward<Args>(args) << " ", static_cast<char>(0))... };

        prepare_log(strm, log_level);
    }

    void auto_log(const std::string& text, const bool from_autolog);
    void clear_last_function_name();

private:
    std::string getCurrentTimeAndDate();
    std::string getCurrentThread();
    std::string convertLogLevelToString(const LOG_LEVEL log_level);
    void prepare_log(const std::ostringstream& text, const LOG_LEVEL log_level);
    void show_and_save_to_file_logs(const std::string& log);

private:
    std::string logger_name_;
    std::string file_name_or_path_;
    std::vector<std::string> called_functions_;
};

class AutoTrace {
public:
    AutoTrace(LoggerPtr logger_ptr, const char *function_name);
    ~AutoTrace();

private:
    LoggerPtr logger_ptr_;
    std::string function_name_;
};

#endif // LOGGER_H
