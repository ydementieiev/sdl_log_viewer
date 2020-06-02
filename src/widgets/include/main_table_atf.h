#ifndef MAIN_TABLE_H
#define MAIN_TABLE_H

#include "table.h"

class MainTableATF : public Table {
public:
    MainTableATF(QTableView *table, QString table_name);
    void findMatchNext();
    void findMatchPrev();

    void readFromFile(const QString &file_path);
    bool findAllMatch(const QString &plain_text);
    void copyMatchFromOtherTable(Table *other_table);
    QTableView* getCurrentTableWidget();
    void checkClickedTitleRow(QModelIndex clicked_row);
    void openTitleRowAfterSearch(int row);
    void copyOneToNotes(Table *other_table);
//    QList<QTableWidgetItem *> qlist_;
    std::vector<int> qlist_vector_;
    void hideColumns(int state, QString column);
    void hideEmptyRow(int state);
    void setFlagForSearch(Qt::CaseSensitivity flag_for_search);
    void clearTable();
    int getFindedItemIndex();
    int getFindedItemCount();
    bool hasOpenRow();
    void closeAllOpenRow();
    void scrollToItemInOtherTable(Table *other_table, const QModelIndex &index);
    virtual std::vector<int> getQlistVector() {
        return qlist_vector_;
    }
private:
    void parceLineToTable(QString &line, int row);
    void resizeColumnsToContents();
    void calculateLogsInStep();
//    QTableView *table_;
//    QStandardItemModel *model;
//    std::vector<Title> title_rows;
//    int index_of_scroll_;
//    Qt::CaseSensitivity flag_for_search_;
};

#endif // MAIN_TABLE_H
