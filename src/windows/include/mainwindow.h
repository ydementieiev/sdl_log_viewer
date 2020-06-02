#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "main_table_atf.h"

#include "configwindow.h"
#include "consolescreen.h"
#include "button_hide_all_opened_rows.h"
#include "default_config.h"

#include <QMainWindow>
#include <QShortcut>
#include <QFileSystemModel>
#include <QCheckBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();

    void slotShortcutCtrlF();
    void slotShortcutCtrlLeftMouse();
    void slotShortcutLeftMouse();
    void slotShortcutDel();

    void on_button_add_to_notes_clicked();
    void on_button_Find_clicked();
    void on_toolButton_caseSensetive_clicked();
    void on_toolButton_RegularExpression_clicked();
    void on_filesView_doubleClicked(const QModelIndex &index);
    void on_main_table_widget__clicked(const QModelIndex &index);
    void on_actionConfig_triggered();
    void on_hide_all_clicked();
    void on_apply_filter_clicked();
    void on_toolButton_findNext_clicked();
    void on_toolButton_findPrev_clicked();
    void checkboxStateChanged(int state);

    void on_search_table_widget__clicked(const QModelIndex &index);
    void on_notes_table_widget__clicked(const QModelIndex &index);
    void on_serch_input_for_filter_textChanged(const QString &arg1);

    void on_button_apply_filter_clicked();

    void on_toolButton_check_uncheck_all_clicked();

    void on_button_hide_all_clicked();

private:
    Ui::MainWindow *ui;
    Table *main_table_;
    Table *search_table_;
    Table *notes_table_;

    QString search_text_;
    QFileSystemModel *file_system_model_;

    Button_hideAllOpenedRows *button_hideAllOpenedRows_;
    ConfigWindow c;
    ConsoleScreen consoleScreen;
    Config config;
    QList<QCheckBox*> m_checkboxes;

    QShortcut *keyCtrlF;  // Ctrl + F
    QShortcut *keyEnter;  // Ctrl + F
    QShortcut *keyF3;  // F3
    QShortcut *keyCtrlLeftMouse;  // Ctrl + LeftMouse
    QShortcut *keyDel;  // Ctrl + LeftMouse
};

#endif // MAINWINDOW_H
