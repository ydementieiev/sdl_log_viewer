#include "configwindow.h"
#include "default_config.h"
#include "ui_configwindow.h"
#include "logger.h"

CREATE_LOGGERPTR_GLOBAL(logger_ptr, "ConfigWindow", "configwindow.cpp");

ConfigWindow::ConfigWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConfigWindow)
{
    LOG_AUTO_TRACE(logger_ptr, "ConfigWindow::ConfigWindow");
    ui->setupUi(this);
    ui->lineEdit->setText(Config().get_path_to_atf_log_folder());
}

ConfigWindow::~ConfigWindow()
{
    delete ui;
}
