#ifndef CONSOLESCREEN_H
#define CONSOLESCREEN_H

#include <QMainWindow>

namespace Ui {
class ConsoleScreen;
}

class ConsoleScreen : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConsoleScreen(QWidget *parent = nullptr);
    void readFile(QString file_path);

    ~ConsoleScreen();

private:
    Ui::ConsoleScreen *ui;
};

#endif // CONSOLESCREEN_H
