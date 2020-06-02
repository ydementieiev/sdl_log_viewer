#ifndef DEFAULT_CONFIG_H
#define DEFAULT_CONFIG_H

#include <QString>
#include <QFile>
#include <QDebug>

const QString CONFIG_FILE = "/home/ydementieiev/Documents/Project/Log_viewer/src/config/config.txt";

enum CONFIG
{
    INVALID = 0,
    PATH_TO_ATF_LOGS = 1
};

class Config {
public:
    Config();
    QString get_path_to_atf_log_folder();
    void set_path_to_atf_log_folder(QString &new_path);

private:
    QString path_to_atf_logs_;
    CONFIG config_to_enum(const QString &str);
};

#endif // DEFAULT_CONFIG_H
