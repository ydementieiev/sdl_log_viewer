#include "main_table_atf.h"

#include "logger.h"
#include "line_parser.h"
#include <QFile>
#include <QDebug>
#include <QStringRef>
#include <QList>
#include <algorithm>
#include <QStringList>
#include <QLabel>
#include <QTextEdit>
#include <QTextCursor>
#include <QBrush>
#include "progress_bar.h"

CREATE_LOGGERPTR_GLOBAL(logger_ptr, "MainTableATF", "main_table_atf.cpp");

namespace {
HEADERS convertHeaderStrToEnum(QString header_name)
{
    if (header_name == "Direction") {
        return HEADERS::DIRECTION;
    }
    if (header_name == "Date") {
        return HEADERS::DATE;
    }
    if (header_name == "Time") {
        return HEADERS::TIME;
    }
    if (header_name == "Logs") {
        return HEADERS::LOGS;
    }
}
}

MainTableATF::MainTableATF(QTableView *table, QString table_name)
    : Table(table, table_name)
{
}

void MainTableATF::readFromFile(const QString &file_path)
{
    LOG_AUTO_TRACE(logger_ptr, "MainTableATF::readFromFile");

    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(logger_ptr, "File not open during read.");
        return;
    }
    int row = 0;

    while (!file.atEnd()) {
        QString line = file.readLine();
        if (!line.startsWith("=====") && !line.startsWith('\n') && !line.startsWith(" ")) {
            line = line.simplified();
            parceLineToTable(line, row);
            row++;
        } else if (line.startsWith("=====")) {
            model->insertRow(row);
            line = line.simplified();
            table_->setSpan(row, 0, 1, model->columnCount());
            model->setItem(row, HEADERS::DIRECTION, new QStandardItem(line));

            Title title = {row, false, false};
            title_rows.push_back(title);
            model->setData(model->index(row, HEADERS::DIRECTION), QBrush( Qt::darkCyan ), Qt::TextColorRole);
            row++;
        }
    }
    calculateLogsInStep();
    resizeColumnsToContents();
    file.close();
}

bool MainTableATF::findAllMatch(const QString &plain_text)
{
    LOG_AUTO_TRACE(logger_ptr, "MainTableATF::findAllMatch");

    LOG_DEBUG(logger_ptr, "Find all matches with with - ", plain_text.toStdString());
    if (!qlist_vector_.empty()) {
        for (auto i = 0; i < qlist_vector_.size(); i++) {
            QTextEdit *bar = dynamic_cast<QTextEdit*>(table_->indexWidget(model->index(qlist_vector_[i], HEADERS::LOGS)));
            QString line_for_parse = bar->toPlainText();
            LineParser parse(line_for_parse);
            table_->setIndexWidget(model->index(i, HEADERS::LOGS), parse.getTextEdit());
        }

    }

    qlist_vector_.clear();
    ProgressBar().setMaximumRange(model->rowCount());
    ProgressBar().setVisibleProgressBar(true);
    for (auto i = 0; i < model->rowCount(); i++) {
        auto it = find_if(title_rows.begin(), title_rows.end(), [&](const Title & s)->bool { return s.title_row == i; } );
        if (it != title_rows.end()) {
            LOG_DEBUG(logger_ptr, "Skip finded text in title");
            continue;
        }

        QTextEdit *bar = dynamic_cast<QTextEdit*>(table_->indexWidget(model->index(i, HEADERS::LOGS)));
        QString line_for_parse = bar->toPlainText();
        if(line_for_parse.contains(plain_text, Qt::CaseInsensitive)) {
            openTitleRowAfterSearch(i);
            qlist_vector_.push_back(i);
            LineParser parse(line_for_parse, plain_text);
            table_->setIndexWidget(model->index(i, HEADERS::LOGS), parse.getTextEdit());
        }
        ProgressBar().setProgress(i);
    }
    ProgressBar().setVisibleProgressBar(false);
    index_of_scroll_ = 0;

    if (qlist_vector_.empty()) {
        LOG_DEBUG(logger_ptr, "Nothing finded");
        return false;
    }
    LOG_DEBUG(logger_ptr, "The matched count after search", qlist_vector_.size());

    findMatchNext();
    return true;
}

void MainTableATF::copyMatchFromOtherTable(Table *other_table)
{
    LOG_AUTO_TRACE(logger_ptr, "MainTableATF::copyMatchFromOtherTable");

    std::vector<int> row_number_to_copy = other_table->getQlistVector();
    model->setRowCount(other_table->model->rowCount());
    for (int i = 0; i < other_table->model->rowCount(); i++) {
        table_->hideRow(i);
    }
    QStringList labels;
    if (row_number_to_copy.empty()) {
        LOG_DEBUG(logger_ptr, "Not find item for copy");
        return;
    }

    for (int i = 0; i < row_number_to_copy.size(); i++) {
        int row = row_number_to_copy[i];        labels << QString::number(row + 1);
        auto it = find_if(other_table->title_rows.begin(), other_table->title_rows.end(), [&](const Title & s)->bool { return s.title_row == row; } );
        table_->showRow(int(row));

        if (it != other_table->title_rows.end()) {
            LOG_DEBUG(logger_ptr, "Skip finded text in title");
            table_->hideRow(int(row));
            continue;
        }
        model->setItem(row, HEADERS::DIRECTION, other_table->model->item(row, HEADERS::DIRECTION)->clone());
        model->setItem(row, HEADERS::DATE, other_table->model->item(row, HEADERS::DATE)->clone());
        model->setItem(row, HEADERS::TIME, other_table->model->item(row, HEADERS::TIME)->clone());

        QTextEdit *bar = dynamic_cast<QTextEdit*>(other_table->getCurrentTableWidget()->indexWidget(other_table->model->index(row, HEADERS::LOGS)));
        QString line_for_parse = bar->toPlainText();
        LineParser parse(line_for_parse);

        table_->setIndexWidget(model->index(row, HEADERS::LOGS), parse.getTextEdit());
    }

    resizeColumnsToContents();
}

void MainTableATF::findMatchNext()
{
    LOG_AUTO_TRACE(logger_ptr, "MainTableATF::findMatchNext");

    if (qlist_vector_.empty()) {
        LOG_DEBUG(logger_ptr, "Nothing finded");
        return;
    }

    LOG_DEBUG(logger_ptr, "Scroll to item = ", index_of_scroll_);

    table_->scrollTo(model->index(qlist_vector_[index_of_scroll_], 0));
    QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows;
    QModelIndex index = table_->model()->index(qlist_vector_[index_of_scroll_], 0);
    table_->selectionModel()->select(index, flags);

    if (index_of_scroll_ < qlist_vector_.size() - 1) {
        index_of_scroll_++;
    } else {
        index_of_scroll_ = 0;
    }
}

void MainTableATF::findMatchPrev()
{
    LOG_AUTO_TRACE(logger_ptr, "MainTableATF::findMatchPrev");

    if (qlist_vector_.empty()) {
        LOG_DEBUG(logger_ptr, "Nothing finded");
        return;
    }
    index_of_scroll_--;
    if (index_of_scroll_ < 0) {
        index_of_scroll_ = qlist_vector_.size() - 1;
    }
    LOG_DEBUG(logger_ptr, "Scroll to item = ", index_of_scroll_);
    table_->scrollTo(model->index(qlist_vector_[index_of_scroll_], 0));
//    m_checkboxes[i]->setStyleSheet("border: 0.5px solid; border-color: rgb(193, 125, 17);");
}

void MainTableATF::scrollToItemInOtherTable(Table *other_table, const QModelIndex &index)
{
    LOG_AUTO_TRACE(logger_ptr, "MainTableATF::scrollToItemInOtherTable");
    other_table->table_->scrollTo(other_table->model->index(index.row(), 0));

    QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows;
    QModelIndex index_for_select = other_table->table_->model()->index(index.row(), 0);

    other_table->table_->selectionModel()->select(index_for_select, flags);
//    other_table->index_of_scroll_ = scroll_to;
}

QTableView* MainTableATF::getCurrentTableWidget()
{
    return table_;
}

void MainTableATF::checkClickedTitleRow(QModelIndex clicked_row)
{
    LOG_AUTO_TRACE(logger_ptr, "MainTableATF::checkClickedTitleRow");

    int row_number = clicked_row.row();
    auto it = find_if(title_rows.begin(), title_rows.end(), [&](const Title & s)->bool { return s.title_row == row_number; } );

    if (it != title_rows.end()) {
        int row_to_show = row_number + 1;
        if(!it->open_row) {
            it->open_row = true;
            it++;
            while(row_to_show < it->title_row) {
                table_->showRow(row_to_show);
                row_to_show++;
            }
        } else {
            it->open_row = false;
            it++;
            while(row_to_show < it->title_row) {
                table_->hideRow(row_to_show);
                row_to_show++;
            }
        }
        resizeColumnsToContents();
    }
}

void MainTableATF::openTitleRowAfterSearch(int row)
{
    LOG_AUTO_TRACE(logger_ptr, "MainTableATF::openTitleRowAfterSearch");

    auto it = find_if(title_rows.begin(), title_rows.end(), [&](const Title & s)->bool { return s.title_row > row; } );
    if (it != title_rows.end()) {
        it--;
        if (!it->open_row) {
            it->open_row = true;
            int row_to_show = it->title_row + 1;
            it++;
            while(row_to_show < it->title_row) {
                table_->showRow(row_to_show);
                row_to_show++;
            }
        }
        resizeColumnsToContents();
    }
}


void MainTableATF::copyOneToNotes(Table *other_table)
{
    LOG_AUTO_TRACE(logger_ptr, "MainTableATF::copyOneToNotes");

    int index = other_table->getCurrentTableWidget()->selectionModel()->currentIndex().row();
    auto it = find_if(other_table->title_rows.begin(), other_table->title_rows.end(), [&](const Title & s)->bool { return s.title_row == index; } );
    if (it != other_table->title_rows.end()) {
        LOG_DEBUG(logger_ptr, "Skip copy one to notes");
        return;
    }

    model->insertRow(0);

    model->setItem(0, HEADERS::DIRECTION, other_table->model->item(index, HEADERS::DIRECTION)->clone());
    model->setItem(0, HEADERS::DATE, other_table->model->item(index, HEADERS::DATE)->clone());
    model->setItem(0, HEADERS::TIME, other_table->model->item(index, HEADERS::TIME)->clone());

    QTextEdit *bar = dynamic_cast<QTextEdit*>(other_table->getCurrentTableWidget()->indexWidget(other_table->model->index(index, HEADERS::LOGS)));
    QString line_for_parse = bar->toPlainText();
    LineParser parse(line_for_parse);

    table_->setIndexWidget(model->index(0, HEADERS::LOGS), parse.getTextEdit());

    resizeColumnsToContents();
}

void MainTableATF::hideColumns(int state, QString column)
{
    LOG_AUTO_TRACE(logger_ptr, "MainTableATF::hideColumns");

    if (state == Qt::Unchecked) {
        table_->hideColumn(convertHeaderStrToEnum(column));
    } else if (state == Qt::Checked) {
        table_->showColumn(convertHeaderStrToEnum(column));
    }
}

void MainTableATF::hideEmptyRow(int state)
{
    LOG_AUTO_TRACE(logger_ptr, "MainTableATF::hideEmptyRow");

    if (state == Qt::Unchecked) {
        for (unsigned long i = 0; i < title_rows.size(); i++) {
            if(title_rows[i].empty_row == true) {
                table_->hideRow(title_rows[i].title_row);
            }
        }
        table_->hideRow(title_rows[title_rows.size() - 1].title_row);
    } else if (state == Qt::Checked) {
        for (unsigned long i = 0; i < title_rows.size(); i++) {
            if(title_rows[i].empty_row == true) {
                table_->showRow(title_rows[i].title_row);
            }
        }
        table_->showRow(title_rows[title_rows.size() - 1].title_row);
    }
}

void MainTableATF::setFlagForSearch(Qt::CaseSensitivity flag_for_search)
{
//    flag_for_search_ = flag_for_search;
}

void MainTableATF::clearTable()
{
    model->setRowCount(0);
}

int MainTableATF::getFindedItemIndex()
{
    return index_of_scroll_;
}

int MainTableATF::getFindedItemCount()
{
    return int(qlist_vector_.size());
}

void MainTableATF::parceLineToTable(QString &line, int row)
{
    model->insertRow(row);
    model->setItem(row, HEADERS::DIRECTION, new QStandardItem(line.section(' ', HEADERS::DIRECTION, HEADERS::DIRECTION)));

    model->setItem(row, HEADERS::DATE, new QStandardItem(line.section(' ', HEADERS::DATE, HEADERS::DATE) + "]"));
    model->setData(model->index(row, HEADERS::DATE), QBrush( Qt::darkGreen ), Qt::TextColorRole);

    model->setItem(row, HEADERS::TIME, new QStandardItem("[" + line.section(' ', HEADERS::TIME, HEADERS::TIME)));
    model->setData(model->index(row, HEADERS::TIME), QBrush( Qt::darkGreen ), Qt::TextColorRole);

    QString line_for_parse = line.section(' ', HEADERS::LOGS);
    LineParser parse(line_for_parse);

    table_->setIndexWidget(model->index(row, HEADERS::LOGS), parse.getTextEdit());
    table_->hideRow(row);
}

void MainTableATF::resizeColumnsToContents()
{
    LOG_AUTO_TRACE(logger_ptr, "MainTableATF::resizeColumnsToContents");
    table_->resizeColumnToContents(HEADERS::DATE);
    table_->resizeColumnToContents(HEADERS::TIME);
    table_->resizeColumnToContents(HEADERS::LOGS);
}

void MainTableATF::calculateLogsInStep()
{
    LOG_AUTO_TRACE(logger_ptr, "MainTableATF::calculateLogsInStep");

    for (unsigned long i = 0; i < title_rows.size() - 1; i++) {
        QString str;
        int row_count = title_rows[i + 1].title_row - title_rows[i].title_row;
        str.setNum(row_count - 1);
        if (row_count <= 1) {
            LOG_INFO(logger_ptr, "Row number are empty");
            str = "Empty";
            title_rows[i].empty_row = true;
            model->item(title_rows[i].title_row, HEADERS::DIRECTION)->setFlags(Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsSelectable);
        }
        if(!model->item(title_rows[i].title_row, HEADERS::DIRECTION)->text().startsWith("===== ---")) {
            model->item(title_rows[i].title_row, HEADERS::DIRECTION)->setText(model->item(title_rows[i].title_row, HEADERS::DIRECTION)->text() + " Step size = " + str);
            model->item(title_rows[i].title_row, HEADERS::DIRECTION)->setFlags(Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsSelectable);
        }
    }
    model->item(title_rows[title_rows.size() - 1].title_row, HEADERS::DIRECTION)->setFlags(Qt::ItemIsDragEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsSelectable);
}

bool MainTableATF::hasOpenRow()
{
    LOG_AUTO_TRACE(logger_ptr, "MainTableATF::hasOpenRow");

    unsigned long open_title_count = 0;
    for (unsigned long i = 0; i < title_rows.size() - 1; i++) {
        if (title_rows[i].open_row) {
            open_title_count++;
        }
    }
    return (open_title_count != 0);
}

void MainTableATF::closeAllOpenRow()
{
    LOG_AUTO_TRACE(logger_ptr, "MainTableATF::closeAllOpenRow");

    for (unsigned long i = 0; i < title_rows.size() - 1; i++) {
        if (title_rows[i].open_row) {
            int row_to_show = title_rows[i].title_row + 1;
            title_rows[i].open_row = false;
            while(row_to_show < title_rows[i+1].title_row) {
                table_->hideRow(row_to_show);
                row_to_show++;
            }
        }
    }
}
