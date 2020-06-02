#ifndef MOCK_TABLE_H
#define MOCK_TABLE_H

#include <gmock/gmock.h>
#include "table.h"

class Mock_Table : public Table
{
public:
    Mock_Table(QTableView *table)
        : Table (table){};
    virtual ~Mock_Table() {};

    MOCK_METHOD0(findMatchNext, void());
    MOCK_METHOD0(findMatchPrev, void());
    MOCK_METHOD1(readFromFile, void(const QString &file_path));
    MOCK_METHOD1(findAllMatch, bool(const QString &plain_text));
    MOCK_METHOD1(checkClickedTitleRow, void(QModelIndex clicked_row));
    MOCK_METHOD1(openTitleRowAfterSearch, void(int row));
    MOCK_METHOD0(getSelectedRowIndex, int());
    MOCK_METHOD2(hideColumns, void(int state, int column));
    MOCK_METHOD1(hideEmptyRow, void(int state));
    MOCK_METHOD1(setFlagForSearch, void(Qt::CaseSensitivity flag_for_search));
    MOCK_METHOD0(getFindedItemIndex, int());
    MOCK_METHOD0(getFindedItemCount, int());
    MOCK_METHOD0(resizeColumnsToContents, void());
    MOCK_METHOD0(calculateLogsInStep, void());
};

#endif //MOCK_TABLE_H
