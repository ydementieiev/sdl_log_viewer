#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "table.h"
#include <QTableView>
#include "mock/mock_line_parser.h"

using namespace testing;
using ::testing::_;
using ::testing::ReturnRef;
using ::testing::NiceMock;
//typedef NiceMock<Mock_LineParser> MockLineParser;

const QString str = "sting";
class Table_test : public ::testing::Test
{
public:
    Table_test() { /* init protected members here */ }
    ~Table_test() { /* free protected members here */ }
    void SetUp() { /* called before every test */ }
    void TearDown() { /* called after every test */ }

    Ui::MainWindow *ui;
    QTableView *main_table;
};

TEST_F(Table_test, testAll)
{
    main_table = new QTableView();
    Table table(main_table);

    EXPECT_EQ(1, 1);
}
