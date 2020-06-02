#include "logger.h"

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <thread>
#include <sstream>

namespace  {
const std::string ERROR_STR = "[ERROR] ";
const std::string INFO_STR  = "[INFO*] ";
const std::string DEBUG_STR = "[DEBAG] ";

const std::string ENTER_STR = " -> Enter";
const std::string EXIT_STR  = " <- Exit";
}

static std::ofstream fout("log.txt");

Logger::Logger(const char *logger_name, const char *file_name_or_path)
    : logger_name_(logger_name)
    , file_name_or_path_(file_name_or_path)
{

    logger_name_.insert(logger_name_.begin(), '[');
    logger_name_.insert(logger_name_.end(), ']');
    logger_name_.insert(logger_name_.end(), ' ');

    file_name_or_path_.insert(file_name_or_path_.begin(), '[');
    file_name_or_path_.insert(file_name_or_path_.end(), ']');
    file_name_or_path_.insert(file_name_or_path_.end(), ' ');
}

Logger::~Logger()
{
}

void Logger::auto_log(const std::string& active_function_name, const bool entered_to_new_function)
{
    if (entered_to_new_function) {
        std::string function_name = active_function_name + ": ";
        called_functions_.push_back(function_name);
    }

    std::string  auto_log_str =
          INFO_STR
        + getCurrentTimeAndDate()
        + getCurrentThread()
        + logger_name_
        + file_name_or_path_
        + active_function_name
        + (entered_to_new_function ? ENTER_STR : EXIT_STR);

    show_and_save_to_file_logs(auto_log_str);
}

void Logger::clear_last_function_name()
{
    if (!called_functions_.empty()) {
        called_functions_.erase(called_functions_.end());
    }
}

std::string Logger::getCurrentTimeAndDate()
{
    time_t rawtime;
    struct tm * timeinfo;

    time( &rawtime );
    timeinfo = localtime ( &rawtime );

    std::string current_time(asctime (timeinfo));
    current_time.insert(current_time.begin(), '[');
    current_time.erase(current_time.end() - 1);
    current_time.insert(current_time.end(), ']');
    current_time.insert(current_time.end(), ' ');

    return current_time;
}

std::string Logger::getCurrentThread()
{
    std::ostringstream out;
    std::thread::id id = std::this_thread::get_id();
    out << "[" << id << "] ";
    return out.str();
}

std::string Logger::convertLogLevelToString(const LOG_LEVEL log_level)
{
    switch(log_level) {
    case LOG_LEVEL::INFO:
        return INFO_STR;
    case LOG_LEVEL::DEBUG:
        return DEBUG_STR;
    case LOG_LEVEL::ERROR:
        return ERROR_STR;
    }
}

void Logger::prepare_log(const std::ostringstream& message_from_user, const LOG_LEVEL log_level)
{
    std::string user_log = called_functions_.empty()
            ? message_from_user.str()
            : (called_functions_[called_functions_.size()-1] + message_from_user.str());

    std::string  prepared_log_str =
          convertLogLevelToString(log_level)
        + getCurrentTimeAndDate()
        + getCurrentThread()
        + logger_name_
        + file_name_or_path_
        + user_log;

    show_and_save_to_file_logs(prepared_log_str);
}

void Logger::show_and_save_to_file_logs(const std::string& log)
{
    std::cout << log << std::endl;
    fout << log << std::endl;
}

AutoTrace::AutoTrace(LoggerPtr logger_ptr, const char *function_name)
    : logger_ptr_(logger_ptr)
    , function_name_(function_name)
{
    logger_ptr_->auto_log(function_name_, true);
}

AutoTrace::~AutoTrace()
{
    logger_ptr_->auto_log(function_name_, false);
    logger_ptr_->clear_last_function_name();
}
