#include "src/windows/include/mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(QDir::currentPath() + "/images/icon.png"));
    MainWindow w;
    w.showMaximized();

    return a.exec();
}
