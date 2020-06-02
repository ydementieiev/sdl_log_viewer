#include "default_config.h"
#include "logger.h"

CREATE_LOGGERPTR_GLOBAL(logger_ptr, "Config", "default_config.cpp");

Config::Config()
    :path_to_atf_logs_(QString())
{
    LOG_AUTO_TRACE(logger_ptr, "Config::Config");
    QFile file(CONFIG_FILE);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(logger_ptr, "File not open during read.");
        return;
    }
    while (!file.atEnd()) {
        QString line = file.readLine();

        switch(config_to_enum(line.section(' ', 0, 0))) {
        case PATH_TO_ATF_LOGS:
            path_to_atf_logs_ = line.section('"', 1, 1);
            LOG_DEBUG(logger_ptr, "Read from config file, path_to_atf_logs_ = ", path_to_atf_logs_.toStdString());
            break;
        default:
            break;
        }
    }
}

CONFIG Config::config_to_enum(const QString &str)
{
    if (str.startsWith("path_to_atf_logs")) {
        return PATH_TO_ATF_LOGS;
    }
    return INVALID;
}

QString Config::get_path_to_atf_log_folder()
{
    return path_to_atf_logs_;
}

void Config::set_path_to_atf_log_folder(QString &new_path)
{
    LOG_AUTO_TRACE(logger_ptr, "Config::set_path_to_atf_log_folder");

    QFile file(CONFIG_FILE);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        LOG_ERROR(logger_ptr, "File not open during read.");
        return;
    }
    while (!file.atEnd()) {
        QString line = file.readLine();
        switch(config_to_enum(line.section(' ', 0, 0))) {
        case PATH_TO_ATF_LOGS:
            path_to_atf_logs_ = line.section('"', 1, 1);
            LOG_DEBUG(logger_ptr, "Set path to atf folder, path_to_atf_logs_ = ", path_to_atf_logs_.toStdString());
            break;
        default:
            break;
        }
    }
}
