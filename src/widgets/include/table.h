#ifndef TABLE_H
#define TABLE_H

#include "ui_mainwindow.h"

#include <QTableWidget>
#include <QStandardItemModel>

#include <vector>

#include "filter_tree.h"

const QStringList headers = {
     "Direction"
    ,"Date"
    ,"Time"
    ,"Logs" };

const QStringList HEADERS_STR = {
     "Level"
    ,"Date and Time"
    ,"Thread"
    ,"Logger Name"
    ,"Path"
    ,"Log"};

enum HEADERS {
     DIRECTION = 0
    ,DATE = 1
    ,TIME = 2
    ,LOGS = 3 };

struct Title {
    int title_row;
    bool open_row;
    bool empty_row;
};

enum HEADERS_TABLE {
     LEVEL = 0
    ,DATE_TIME = 1
    ,THREAD = 2
    ,LOGGER_NAME = 3
    ,PATH = 4
    ,LOG = 5};

struct BindingRowNumbers
{
    int row_main;
    int row_search;
};

class Table {
public:
    Table(QTableView *table, QString table_name = "No name table");
    virtual ~Table() = default;
    virtual void findMatchNext();
    virtual void findMatchPrev();

    virtual void readFromFile(const QString &file_path);
    virtual bool findAllMatch(const QString &plain_text);
    virtual void copyMatchFromOtherTable(Table *other_table);
    QTableView* getCurrentTableWidget();
    int getSelectedRowIndex();
    virtual void copyOneToNotes(Table *other_table);
    virtual void hideColumns(int state, QString column);
    QList<QTableWidgetItem *> qlist_;
    std::vector<int> qlist_vector_;
    QList<QStandardItem*> founded_items_;

    virtual void scrollToItemInOtherTable(Table *other_table, const QModelIndex &index);
    void setFlagForSearch(Qt::MatchFlag flag_for_search);
    void clearTable();
    int getFindedItemIndex();
    int getFindedItemCount();

    void checkAndSaveFilterValue(QString filter_name, QString filter_value);
    void applyFilter();
    void applyFilterPressed();
    void filter_search_text_changed(const QString &new_str);
    void pressedCheckUncheckAllFilter();

    virtual std::vector<int> getQlistVector(){}
    virtual void checkClickedTitleRow(QModelIndex clicked_row){}
    virtual void openTitleRowAfterSearch(int row){}
    virtual bool hasOpenRow(){}
    virtual void hideEmptyRow(int state){}
    virtual void closeAllOpenRow(){}
public:
    void parceLineToTable(QString &line, int row);
//    HEADERS_TABLE convertHeaderStrToEnum(QString header_name);

    void resizeColumnsToContents();
    QTableView *table_;
    QStandardItemModel *model;
    std::vector<Title> title_rows;
    int index_of_scroll_;
    QFlags<Qt::MatchFlag> flag_for_search_;
    QString table_name_;
    std::vector<std::string> level_filter_value;
    std::vector<std::string> thread_filter_value;
    std::vector<std::string> logger_name_filter_value;
    std::map<int,int> binding_row_numbers_;
    FilterTree file_tree;
};

#endif // TABLE_H
