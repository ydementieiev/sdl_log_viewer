#include "consolescreen.h"
#include "ui_consolescreen.h"
#include "logger.h"

#include <QFile>

CREATE_LOGGERPTR_GLOBAL(logger_ptr, "ConsoleScreen", "consolescreen.cpp");

ConsoleScreen::ConsoleScreen(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ConsoleScreen)
{
    ui->setupUi(this);
}


void ConsoleScreen::readFile(QString file_path)
{
    LOG_AUTO_TRACE(logger_ptr, "ConsoleScreen::readFile");

    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(logger_ptr, "File not open during read.");
        return;
    }

//    while (!file.atEnd()) {
    QString line = file.readAll();
    ui->textEdit->setText(line);
//        QString line = file.readLine();
//        if (!line.startsWith('\n') && !line.startsWith(" ") && !read_json) {
//            line = line.simplified();
//            parceLineToTable(line, row);
//            row++;
//            if (line.endsWith('{')/* || line.endsWith(':')*/) {
//               read_json = true;
//               continue;
//            }
//        }


    file.close();
}

ConsoleScreen::~ConsoleScreen()
{
    delete ui;
}
