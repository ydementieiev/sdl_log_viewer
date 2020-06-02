#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "progress_bar.h"
#include "default_config.h"
#include "logger.h"
#include "filter_tree.h"

#include <QDebug>
#include <QStringList>
#include <QByteArray>
#include <QString>
#include <QTextStream>
#include <QBrush>
#include <QColor>
#include <QList>
#include <QKeyEvent>
#include <QTreeWidgetItem>

CREATE_LOGGERPTR_GLOBAL(logger_ptr, "MainWindow", "mainwindow.cpp");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    search_text_(""),
    file_system_model_(new QFileSystemModel)
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::MainWindow");
    ui->setupUi(this);
    ui->splitter_2->setSizes(QList<int>() << 70 << 30);

    file_system_model_->setRootPath(config.get_path_to_atf_log_folder());
    ui->filesView->setUniformRowHeights(true);
    ui->filesView->setModel(file_system_model_);
    ui->filesView->setRootIndex(file_system_model_->index(config.get_path_to_atf_log_folder()));
    ui->filesView->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->filesView->header()->hideSection(2);

    keyEnter = new QShortcut(ui->search_input_field);
    button_hideAllOpenedRows_ = new Button_hideAllOpenedRows(ui->button_hide_all);

    keyEnter->setKey(Qt::Key_Return);
    connect(keyEnter, SIGNAL(activated()), this, SLOT(on_button_Find_clicked()));

    keyCtrlF = new QShortcut(this);
    keyCtrlF->setKey(Qt::CTRL + Qt::Key_F);
    connect(keyCtrlF, SIGNAL(activated()), this, SLOT(slotShortcutCtrlF()));

    keyCtrlLeftMouse = new QShortcut(ui->main_table_widget_);
    keyCtrlLeftMouse->setKey(Qt::CTRL + Qt::LeftButton);

    keyDel = new QShortcut(ui->notes_table_widget_);
    keyDel->setKey(Qt::Key_Delete);
    connect(keyDel, SIGNAL(activated()), this, SLOT(slotShortcutDel()));

//    connect(keyCtrlLeftMouse, SIGNAL(activated()), this, SLOT(slotShortcutCtrlLeftMouse()));
    connect(ui->main_table_widget_, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(slotShortcutCtrlLeftMouse())); //change to ctrl + leftMause from doubleClicked
    connect(ui->main_table_widget_, SIGNAL(clicked(QModelIndex)), this, SLOT(slotShortcutLeftMouse())); //change
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::checkboxStateChanged(int state)
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::checkboxStateChanged");
    QCheckBox* checkbox = qobject_cast<QCheckBox*>(sender());
    if (checkbox)
    {
        LOG_INFO(logger_ptr, "CheckBox -",checkbox->text().toStdString());
        if ("Show empty titles" == checkbox->text()) {
            main_table_->hideEmptyRow(state);
            return;
        }
        main_table_->hideColumns(state, checkbox->text());
        search_table_->hideColumns(state, checkbox->text());
        notes_table_->hideColumns(state, checkbox->text());
    }
}

void MainWindow::on_button_Find_clicked()
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_button_Find_clicked");

    QString new_search_text = ui->search_input_field->text();
    new_search_text = new_search_text.simplified();
    if (new_search_text.isEmpty()) {
        LOG_DEBUG(logger_ptr, "Search string is empty, clear input field");
        ui->search_input_field->clear();
        return;
    }

    if (new_search_text != search_text_) {
        LOG_DEBUG(logger_ptr, "Search new string");

        ui->search_tabWidget_->setCurrentIndex(0);
        search_table_->clearTable();
        search_text_ = new_search_text;
        if (main_table_->findAllMatch(new_search_text)) {
            ui->label_search_count->setText(QString::number(main_table_->getFindedItemIndex()) + " of " + QString::number(main_table_->getFindedItemCount()));
            search_table_->copyMatchFromOtherTable(main_table_);
        }
    } else {
        LOG_DEBUG(logger_ptr, "Search the same string");
        main_table_->findMatchNext();
        ui->label_search_count->setText(QString::number(main_table_->getFindedItemIndex()) + " of " + QString::number(main_table_->getFindedItemCount()));
    }
    if (main_table_->table_name_ == "MainTableParentatf") {
        button_hideAllOpenedRows_->button_visibility(main_table_->hasOpenRow());
    }
}

void MainWindow::slotShortcutCtrlF()
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::slotShortcutCtrlF");

//    ui->search_input_field_->selectAll();
}

void MainWindow::slotShortcutCtrlLeftMouse()
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::slotShortcutCtrlLeftMouse");

//    on_button_add_to_notes_clicked();
}

void MainWindow::slotShortcutLeftMouse()
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::slotShortcutLeftMouse");
}

void MainWindow::slotShortcutDel()
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::slotShortcutDel");

}

void MainWindow::on_button_add_to_notes_clicked()
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_button_add_to_notes_clicked");

    int index = main_table_->getSelectedRowIndex();
    LOG_DEBUG(logger_ptr, "Selected index =  ", index);

    notes_table_->copyOneToNotes(main_table_);
    ui->search_tabWidget_->setCurrentIndex(1);
}

void MainWindow::on_toolButton_caseSensetive_clicked()
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_toolButton_caseSensetive_clicked");

    search_text_ = "";
    if (ui->toolButton_caseSensetive->isChecked()) {
        main_table_->setFlagForSearch(Qt::MatchCaseSensitive);
    } else {
        main_table_->setFlagForSearch(Qt::MatchContains);
    }
}

void MainWindow::on_toolButton_RegularExpression_clicked()
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_toolButton_RegularExpression_clicked");

    search_text_ = "";
    if (ui->toolButton_RegularExpression->isChecked()) {
        main_table_->setFlagForSearch(Qt::MatchRegExp);
    } else {
        main_table_->setFlagForSearch(Qt::MatchContains);
    }
}

void MainWindow::on_filesView_doubleClicked(const QModelIndex &index)
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_filesView_doubleClicked");

    QString file_path = file_system_model_->filePath(index);
    LOG_DEBUG(logger_ptr, "File path =   ", file_path.toStdString());
    ui->treeWidget->clear();
    ui->tree_winget_filter->clear();
    if (file_path.endsWith("SmartDeviceLinkCore.log")) {
        main_table_ = new Table(ui->main_table_widget_, QString("MainTable"));
        search_table_ = new Table(ui->search_table_widget_, QString("SearchTable"));
        notes_table_ = new Table(ui->notes_table_widget_, QString("NotesTable"));
        QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(ui->treeWidget);
        ui->treeWidget->addTopLevelItem(topLevelItem);
        topLevelItem->setText(0,"Header visibility");
        ProgressBar(ui->progressBar);
        FilterTree filter_tree(ui->tree_winget_filter, ui->serch_input_for_filter);
        for(int i = 0; i < HEADERS_STR.size(); i++) {
            QCheckBox* checkbox = new QCheckBox(HEADERS_STR[i]);
            checkbox->setChecked(true);
            m_checkboxes.append(checkbox);
            ui->treeWidget->setItemWidget(new QTreeWidgetItem(topLevelItem), 0, checkbox);
            connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(checkboxStateChanged(int)));
        }
    } else if (file_path.endsWith("Console.txt") || file_path.endsWith("Report.txt")) {
        consoleScreen.showNormal();
        consoleScreen.readFile(file_path);
        return;
    } else if (file_path.endsWith(".txt")) {
        main_table_ = new MainTableATF(ui->main_table_widget_, QString("MainTableParentatf"));
        search_table_ = new MainTableATF(ui->search_table_widget_, QString("SearchTableParentatf"));
        notes_table_ = new MainTableATF(ui->notes_table_widget_, QString("NotesTableParentatf"));
        QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(ui->treeWidget);
        ui->treeWidget->addTopLevelItem(topLevelItem);
        topLevelItem->setText(0,"Header visibility");
        ProgressBar(ui->progressBar);
        FilterTree filter_tree(ui->tree_winget_filter, ui->serch_input_for_filter);
        for(int i = 0; i < headers.size(); i++) {
            QCheckBox* checkbox = new QCheckBox(headers[i]);
            checkbox->setChecked(true);
            m_checkboxes.append(checkbox);
            ui->treeWidget->setItemWidget(new QTreeWidgetItem(topLevelItem), 0, checkbox);
            connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(checkboxStateChanged(int)));
        }
        QCheckBox* checkbox = new QCheckBox("Show empty titles");
        checkbox->setChecked(false);
        m_checkboxes.append(checkbox);
        ui->treeWidget->setItemWidget(new QTreeWidgetItem(topLevelItem), 0, checkbox);
        connect(checkbox, SIGNAL(stateChanged(int)), this, SLOT(checkboxStateChanged(int)));
    } else {
        return;
    }

    main_table_->clearTable();
    main_table_->readFromFile(file_path);
    if (main_table_->table_name_ == "MainTableParentatf") {
        main_table_->hideEmptyRow(Qt::Unchecked);
    }
}

void MainWindow::on_main_table_widget__clicked(const QModelIndex &index)
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_main_table_widget__clicked");
    if (main_table_->table_name_ == "MainTableParentatf") {
        main_table_->checkClickedTitleRow(index);
        button_hideAllOpenedRows_->button_visibility(main_table_->hasOpenRow());
    }
}

void MainWindow::on_actionConfig_triggered()
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_actionConfig_triggered");

//    this->hide();
    c.showNormal();
}

void MainWindow::on_hide_all_clicked()
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_hide_all_clicked");

    button_hideAllOpenedRows_->button_visibility(false);
    main_table_->closeAllOpenRow();
}

void MainWindow::on_apply_filter_clicked()
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_apply_filter_clicked");
    main_table_->applyFilterPressed();
    search_table_->applyFilterPressed();

}

void MainWindow::on_toolButton_findNext_clicked()
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_toolButton_findNext_clicked");

    main_table_->findMatchNext();
}

void MainWindow::on_toolButton_findPrev_clicked()
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_toolButton_findPrev_clicked");

    main_table_->findMatchPrev();
}

void MainWindow::on_search_table_widget__clicked(const QModelIndex &index)
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_search_table_widget__clicked");
    search_table_->scrollToItemInOtherTable(main_table_, index);
}

void MainWindow::on_notes_table_widget__clicked(const QModelIndex &index)
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_search_table_widget__clicked");
    notes_table_->scrollToItemInOtherTable(main_table_, index);
}

void MainWindow::on_serch_input_for_filter_textChanged(const QString &arg1)
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_serch_input_for_filter_textChanged");
    main_table_->filter_search_text_changed(arg1);
}

void MainWindow::on_button_apply_filter_clicked()
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_button_apply_filter_clicked");
    main_table_->applyFilterPressed();
    search_table_->applyFilterPressed();
}

void MainWindow::on_toolButton_check_uncheck_all_clicked()
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_toolButton_check_uncheck_all_clicked");
    main_table_->pressedCheckUncheckAllFilter();
}

void MainWindow::on_button_hide_all_clicked()
{
    LOG_AUTO_TRACE(logger_ptr, "MainWindow::on_button_hide_all_clicked");

    button_hideAllOpenedRows_->button_visibility(false);
    main_table_->closeAllOpenRow();
}
