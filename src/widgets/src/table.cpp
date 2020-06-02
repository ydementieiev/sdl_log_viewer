#include "table.h"

#include <QFile>
#include <QDebug>
#include <QStringRef>
#include <QList>
#include <QStringList>
#include <QLabel>
#include <QTextEdit>
#include <QTextCursor>
#include <QBrush>
#include <algorithm>
#include <QFlags>

#include "logger.h"
#include "line_parser.h"
#include "progress_bar.h"

CREATE_LOGGERPTR_GLOBAL(logger_ptr, "Table", "table.cpp");

namespace {
HEADERS_TABLE convertHeaderStrToEnum(QString header_name)
{
    if (header_name == "Level") {
        return HEADERS_TABLE::LEVEL;
    }
    if (header_name == "Date and Time") {
        return HEADERS_TABLE::DATE_TIME;
    }
    if (header_name == "Thread") {
        return HEADERS_TABLE::THREAD;
    }
    if (header_name == "Logger Name") {
        return HEADERS_TABLE::LOGGER_NAME;
    }
    if (header_name == "Path") {
        return HEADERS_TABLE::PATH;
    }
    if (header_name == "Log") {
        return HEADERS_TABLE::LOG;
    }
}
}

Table::Table(QTableView *table, QString table_name)
    :table_(table)
    ,index_of_scroll_(0)
    ,flag_for_search_(Qt::MatchContains)
    ,table_name_(table_name)
{
    if (table_name.contains("atf", Qt::CaseInsensitive)) {
        model = new QStandardItemModel(0, headers.size(), table_);
    } else {
        model = new QStandardItemModel(0, HEADERS_STR.size(), table_);
    }
    table_->setMouseTracking(true);
    table_->setFocusPolicy(Qt::ClickFocus);
    table_->setAcceptDrops(false);
    table_->setAutoFillBackground(false);
    table_->setStyleSheet(QString::fromUtf8("background-color: rgb(30, 30, 30);\n"
                                            "selection-background-color: rgb(193, 125, 80);\n"
                                            "selection-color: rgb(0, 0, 0);\n"
                                            "font-size: 9pt;\n"
                                            "color: rgb(155, 144, 146);\n" ""));
    table_->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    table_->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    table_->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    table_->horizontalHeader()->setMinimumSectionSize(30);
    table_->horizontalHeader()->setDefaultSectionSize(80);
    table_->verticalHeader()->setMinimumSectionSize(25);
    table_->verticalHeader()->setDefaultSectionSize(25);

    table_->setAutoScroll(false);
    table_->setEditTriggers(QAbstractItemView::DoubleClicked);
    table_->setAlternatingRowColors(false);
    table_->setSelectionMode(QAbstractItemView::NoSelection);
    table_->setSelectionBehavior(QAbstractItemView::SelectItems);
    table_->setTextElideMode(Qt::ElideLeft);
    table_->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    table_->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    table_->setShowGrid(false);
    table_->setGridStyle(Qt::SolidLine);
    table_->setSortingEnabled(false);
    table_->setWordWrap(true);
    table_->setModel(model);
    table_->verticalHeader()->setDefaultAlignment(Qt::AlignTop);    

    if (table_name.contains("atf", Qt::CaseInsensitive)) {
        model->setHorizontalHeaderLabels(headers);
    } else {
        model->setHorizontalHeaderLabels(HEADERS_STR);
    }
    table_->horizontalHeader()->setDefaultAlignment(Qt::AlignTop);
    table_->horizontalHeader()->setStretchLastSection(true);
    table_->horizontalHeader()->setStyleSheet("color: rgb(155, 144, 146);");
    table_->verticalHeader()->setStyleSheet("color: rgb(155, 144, 146);");
    table_->scrollBarWidgets(Qt::AlignAbsolute);
}

void Table::readFromFile(const QString &file_path)
{
    LOG_AUTO_TRACE(logger_ptr, "Table::readFromFile");
    LOG_DEBUG(logger_ptr, "From ", table_name_.toStdString());
    level_filter_value.clear();
    thread_filter_value.clear();
    logger_name_filter_value.clear();
    file_tree.clearTree();

    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR(logger_ptr, "File not open during read.");
        return;
    }
    int row = 0;
    bool read_json = false;
    QString json;
    ProgressBar progress_bar;
    progress_bar.setMaximumRange(100);
    progress_bar.setVisibleProgressBar(true);
    progress_bar.setProgress(50);
    int json_row_count = 0;
    while (!file.atEnd()) {
//        int progress = file.pos()*100/file.size();
//        progress_bar.setProgress(progress);
        QString line = file.readLine();
        if (!line.startsWith('\n') && !line.startsWith(" ") && !read_json) {
            line = line.simplified();
            parceLineToTable(line, row);
            row++;
            if (line.endsWith('{')/* || line.endsWith(':')*/) {
               read_json = true;
               continue;
            }
        } else if (read_json) {
            json_row_count++;
            json += line;
            if (line.startsWith('}')) {
                model->insertRow(row);
//                LOG_DEBUG(logger_ptr, json.toStdString());
                table_->setSpan(row, 0, 1, model->columnCount()); //sets the 1st row 1st column cell to span over all the columns
                model->setItem(row, HEADERS_TABLE::LEVEL, new QStandardItem(json));
                QColor json_text_color = QColor(139, 83, 57);
                model->setData(model->index(row, HEADERS_TABLE::LEVEL), json_text_color, Qt::TextColorRole);
                table_->setRowHeight(row, (table_->rowHeight(row - 1) * 3));
                json.clear();
//                table_->resizeRowToContents(row);
                checkAndSaveFilterValue(QString("Json"), "Json value");
                read_json = false;
                row++;
                json_row_count = 0;
            }
        }
    }
    table_->horizontalHeader()->setStretchLastSection(false);
    resizeColumnsToContents();
    file_tree.createFilter(QString("Level"), level_filter_value);
    file_tree.createFilter(QString("Thread"), thread_filter_value);
    file_tree.createFilter(QString("Logger name"), logger_name_filter_value);

    ProgressBar().setVisibleProgressBar(false);
    file.close();
}

bool Table::findAllMatch(const QString &plain_text)
{
    LOG_AUTO_TRACE(logger_ptr, "Table::findAllMatch");
    LOG_DEBUG(logger_ptr, "From ", table_name_.toStdString());
    LOG_DEBUG(logger_ptr, "Find all matches with with - ", plain_text.toStdString());
    ProgressBar progress_bar;
    progress_bar.setMaximumRange(model->rowCount());
    progress_bar.setVisibleProgressBar(true);
    index_of_scroll_ = 0;
    founded_items_.clear();
    for (int index = model->columnCount() - 1; index < model->columnCount(); index++) {
        progress_bar.setProgress(index);
        founded_items_ = model->findItems(plain_text, flag_for_search_, index);
        int count = founded_items_.count();
        if(count > 0) {
            for(int k = 0; k < count; k++) {
                 QModelIndex modelIndex = model->indexFromItem(founded_items_[k]);
                 if (table_->isRowHidden(modelIndex.row())) {
//                    qDebug()<< "column= " << index << "row=" << modelIndex.row();
//                    founded_items_.removeAt(k);
//                    k--;
                 }
//                 model->item(modelIndex.row(), index)->setData(QBrush(Qt::green), Qt::TextColorRole);
            }
        }
    }
    progress_bar.setVisibleProgressBar(false);

    if (founded_items_.empty()) {
        LOG_DEBUG(logger_ptr, "Nothing finded");
        return false;
    }
    LOG_DEBUG(logger_ptr, "The matched count after search", founded_items_.size());
    findMatchNext();
    return true;
}

void Table::applyFilter()
{
    LOG_AUTO_TRACE(logger_ptr, "Table::applyFilter");

}

void Table::pressedCheckUncheckAllFilter()
{
    LOG_AUTO_TRACE(logger_ptr, "Table::pressedCheckUncheckAllFilter");
    file_tree.checkUncheckAll();
}

void Table::applyFilterPressed()
{
    std::vector<QString> non_active_filter = file_tree.getActiveFilter();
//    for (size_t index = 0; index < non_active_filter.size(); index++) {
////        qDebug() << "column= " << index << "NEED HIDE FILTER=" << non_active_filter[index];
//    }
    for (int i = 0; i < table_->model()->rowCount(); i++) {
        table_->showRow(i);
    }
    for (size_t index = 0; index < non_active_filter.size(); index++) {
        QList<QStandardItem*> need_hide;
        if (non_active_filter[index] == "Json value") {
            need_hide = model->findItems("\t", Qt::MatchStartsWith, 0);
//            QStandardItem *tt = model->item(3,0);
//            qDebug() << "TEST " << tt->text();
        } else {
            need_hide = model->findItems(non_active_filter[index], Qt::MatchContains, 0);
        }
        int count = need_hide.count();
        if(count > 0) {
            for(int k = 0; k < count; k++) {
                 QModelIndex modelIndex = model->indexFromItem(need_hide[k]);
                 if (!table_->isRowHidden(modelIndex.row())) {
//                    qDebug() << "column= " << index << "hide row=" << modelIndex.row();
                    table_->hideRow(modelIndex.row());
                 }
            }
        }
    }
    for (size_t index = 0; index < non_active_filter.size(); index++) {
        QList<QStandardItem*> need_hide;
        if (non_active_filter[index] == "Json value") {
            need_hide = model->findItems("\t", Qt::MatchStartsWith, 2);
//            QStandardItem *tt = model->item(3,0);
//            qDebug() << "TEST " << tt->text();
        } else {
            need_hide = model->findItems(non_active_filter[index], Qt::MatchContains, 2);
        }
        int count = need_hide.count();
        if(count > 0) {
            for(int k = 0; k < count; k++) {
                 QModelIndex modelIndex = model->indexFromItem(need_hide[k]);
                 if (!table_->isRowHidden(modelIndex.row())) {
//                    qDebug() << "column= " << index << "hide row=" << modelIndex.row();
                    table_->hideRow(modelIndex.row());
                 }
            }
        }
    }
    for (size_t index = 0; index < non_active_filter.size(); index++) {
        QList<QStandardItem*> need_hide;
        if (non_active_filter[index] == "Json value") {
            need_hide = model->findItems("\t", Qt::MatchStartsWith, 3);
//            QStandardItem *tt = model->item(3,0);
//            qDebug() << "TEST " << tt->text();
        } else {
            need_hide = model->findItems(non_active_filter[index], Qt::MatchContains, 3);
        }
        int count = need_hide.count();
        if(count > 0) {
            for(int k = 0; k < count; k++) {
                 QModelIndex modelIndex = model->indexFromItem(need_hide[k]);
                 if (!table_->isRowHidden(modelIndex.row())) {
//                    qDebug() << "column= " << index << "hide row=" << modelIndex.row();
                    table_->hideRow(modelIndex.row());
                 }
            }
        }
    }
}

void Table::copyMatchFromOtherTable(Table *other_table)
{
    LOG_AUTO_TRACE(logger_ptr, "Table::copyMatchFromOtherTable");
    LOG_DEBUG(logger_ptr, "From ", table_name_.toStdString());

    if (other_table->founded_items_.empty()) {
        LOG_DEBUG(logger_ptr, "Not find item for copy");
        return;
    }
    LOG_DEBUG(logger_ptr, "Founded items size", other_table->founded_items_.size());
    binding_row_numbers_.clear();
    int new_row = 0;
    for (int i = 0; i < other_table->founded_items_.size(); i++) {

        QModelIndex modelIndex = model->indexFromItem(other_table->founded_items_[i]);
        int row = modelIndex.row();
        if (other_table->table_->isRowHidden(row)) {
//            table_->hideRow(i);
            continue;
        }
        model->insertRow(new_row);
        binding_row_numbers_[new_row] = row;
        LOG_DEBUG(logger_ptr, "row", row, "new_row", new_row);

        model->setItem(new_row, HEADERS_TABLE::LEVEL, other_table->model->item(row, HEADERS_TABLE::LEVEL)->clone());
        model->setItem(new_row, HEADERS_TABLE::DATE_TIME, other_table->model->item(row, HEADERS_TABLE::DATE_TIME)->clone());
        model->setItem(new_row, HEADERS_TABLE::THREAD, other_table->model->item(row, HEADERS_TABLE::THREAD)->clone());
        model->setItem(new_row, HEADERS_TABLE::LOGGER_NAME, other_table->model->item(row, HEADERS_TABLE::LOGGER_NAME)->clone());
        model->setItem(new_row, HEADERS_TABLE::PATH, other_table->model->item(row, HEADERS_TABLE::PATH)->clone());
        model->setItem(new_row, HEADERS_TABLE::LOG, other_table->model->item(row, HEADERS_TABLE::LOG)->clone());
        new_row++;
    }

    resizeColumnsToContents();
}

void Table::scrollToItemInOtherTable(Table *other_table, const QModelIndex &index)
{
    LOG_AUTO_TRACE(logger_ptr, "Table::scrollToItemInOtherTable");
    LOG_DEBUG(logger_ptr, "index.row() ", index.row());
    int scroll_to = binding_row_numbers_[index.row()];
    LOG_DEBUG(logger_ptr, "scroll_to =", scroll_to);
    other_table->table_->scrollTo(other_table->model->index(scroll_to,  HEADERS_TABLE::LEVEL));

    QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows;
    QModelIndex index_for_select = other_table->table_->model()->index(scroll_to, 0);

    other_table->table_->selectionModel()->select(index_for_select, flags);
    other_table->index_of_scroll_ = scroll_to;
}

void Table::findMatchNext()
{
    LOG_AUTO_TRACE(logger_ptr, "Table::findMatchNext");
    LOG_DEBUG(logger_ptr, "From ", table_name_.toStdString());

    if (founded_items_.empty()) {
        LOG_DEBUG(logger_ptr, "Nothing finded");
        return;
    }

    LOG_DEBUG(logger_ptr, "Scroll to item = ", index_of_scroll_);
    QModelIndex modelIndex = model->indexFromItem(founded_items_[index_of_scroll_]);

    table_->scrollTo(model->index(modelIndex.row(), HEADERS_TABLE::LEVEL));
//    table_->selectRow(modelIndex.row());
    QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows;
    QModelIndex index = table_->model()->index(modelIndex.row(), 0);
    table_->selectionModel()->select(index, flags);

    if (index_of_scroll_ < founded_items_.size() - 1) {
        index_of_scroll_++;
    } else {
        index_of_scroll_ = 0;
    }
}

void Table::findMatchPrev()
{
    LOG_AUTO_TRACE(logger_ptr, "Table::findMatchPrev");
    LOG_DEBUG(logger_ptr, "From ", table_name_.toStdString());

    if (founded_items_.empty()) {
        LOG_DEBUG(logger_ptr, "Nothing finded");
        return;
    }
    QModelIndex modelIndex = model->indexFromItem(founded_items_[index_of_scroll_]);

    index_of_scroll_--;
    if (index_of_scroll_ < 0) {
        index_of_scroll_ = founded_items_.size() - 1;
    }
    LOG_DEBUG(logger_ptr, "Scroll to item = ", index_of_scroll_);
    table_->scrollTo(model->index(modelIndex.row(), modelIndex.column()));
    QItemSelectionModel::SelectionFlags flags = QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows;
    QModelIndex index = table_->model()->index(modelIndex.row(), 0);
    table_->selectionModel()->select(index, flags);
}

QTableView* Table::getCurrentTableWidget()
{
    return table_;
}

int Table::getSelectedRowIndex()
{
    LOG_AUTO_TRACE(logger_ptr, "Table::getSelectedRowIndex");
    int index = table_->selectionModel()->currentIndex().row();
    LOG_DEBUG(logger_ptr, table_name_.toStdString(), "index", index);

    return index;
}

void Table::copyOneToNotes(Table *other_table)
{
    LOG_AUTO_TRACE(logger_ptr, "Table::copyOneToNotes");
    LOG_DEBUG(logger_ptr, "From ", table_name_.toStdString());
    model->insertRow(0);

    int index = other_table->getCurrentTableWidget()->selectionModel()->currentIndex().row();
    // ToDo: Change binding_row_numbers_ to correct, now we work corectly only with one item in notes table
    binding_row_numbers_[0] = index;

    model->setItem(0, HEADERS_TABLE::LEVEL, other_table->model->item(index, HEADERS_TABLE::LEVEL)->clone());
    model->setItem(0, HEADERS_TABLE::DATE_TIME, other_table->model->item(index, HEADERS_TABLE::DATE_TIME)->clone());
    model->setItem(0, HEADERS_TABLE::THREAD, other_table->model->item(index, HEADERS_TABLE::THREAD)->clone());
    model->setItem(0, HEADERS_TABLE::LOGGER_NAME, other_table->model->item(index, HEADERS_TABLE::LOGGER_NAME)->clone());
    model->setItem(0, HEADERS_TABLE::PATH, other_table->model->item(index, HEADERS_TABLE::PATH)->clone());
    model->setItem(0, HEADERS_TABLE::LOG, other_table->model->item(index, HEADERS_TABLE::LOG)->clone());
    resizeColumnsToContents();
}

void Table::filter_search_text_changed(const QString &new_str)
{
    LOG_AUTO_TRACE(logger_ptr, "Table::filter_search_text_changed");

    file_tree.find_filter(new_str);
}

void Table::hideColumns(int state, QString column)
{
    LOG_AUTO_TRACE(logger_ptr, "Table::hideColumns");
    LOG_DEBUG(logger_ptr, table_name_.toStdString());

    if (state == Qt::Unchecked) {
        table_->hideColumn(convertHeaderStrToEnum(column));
    } else if (state == Qt::Checked) {
        table_->showColumn(convertHeaderStrToEnum(column));
    }
}

void Table::setFlagForSearch(Qt::MatchFlag flag_for_search)
{
    if (flag_for_search == Qt::MatchCaseSensitive) {
        flag_for_search_ = Qt::MatchContains | Qt::MatchCaseSensitive;
    } else if (flag_for_search == Qt::MatchRegExp){
        flag_for_search_ = Qt::MatchContains | Qt::MatchRegExp;
    } else {
        flag_for_search_ = flag_for_search;
    }
}

void Table::clearTable()
{
    model->setRowCount(0);
}

int Table::getFindedItemIndex()
{
    return index_of_scroll_;
}

int Table::getFindedItemCount()
{
    return int(founded_items_.size());
}

void Table::parceLineToTable(QString &line, int row)
{
//    LOG_AUTO_TRACE(logger_ptr, "Table::parceLineToTable");
    model->insertRow(row);
    int start = 0;
    int i = 0;
//    LOG_DEBUG(logger_ptr, line.toStdString());

    while (line[i] != '\0') {
        if (line[i] == ' ') {
            model->setItem(row, HEADERS_TABLE::LEVEL, new QStandardItem(line.left(i)));
            checkAndSaveFilterValue(QString("Level"), line.left(i));
            break;
        }
        i++;
    }
    i++;
    start = i;

    while (line[i] != '\0') {
        if (line[i] == ']') {
            model->setItem(row, HEADERS_TABLE::DATE_TIME, new QStandardItem(line.mid(start, i+1 - start)));
            model->setData(model->index(row, HEADERS_TABLE::DATE_TIME), QBrush( Qt::darkGreen ), Qt::TextColorRole);
            break;
        }
        i++;
    }
    i++;
    start = i;

    while (line[i] != '\0') {
        if (line[i] == ']') {
            QString thread(line.mid(start, i+1 - start));
//            qDebug() << thread;
            model->setItem(row, HEADERS_TABLE::THREAD, new QStandardItem(thread));
            QColor thread_text_color = QColor(55, 120, 187);
            model->setData(model->index(row, HEADERS_TABLE::THREAD), thread_text_color, Qt::TextColorRole);
            checkAndSaveFilterValue(QString("Thread"), thread);

            break;
        }
     i++;
    }
    i++;
    start = i;

    while (line[i] != '\0') {
        if (line[i] == ']') {
            model->setItem(row, HEADERS_TABLE::LOGGER_NAME, new QStandardItem(line.mid(start, i+1 - start)));
            QColor logger_text_color = QColor(189, 151, 83);
            model->setData(model->index(row, HEADERS_TABLE::LOGGER_NAME), logger_text_color, Qt::TextColorRole);
            checkAndSaveFilterValue(QString("Logger Name"), line.mid(start, i+1 - start));

            break;
         }
        i++;
    }
    i++;
    start = i;

    while (line[i] != '\0') {
        if (line[i+1] == ' ') {
            model->setItem(row, HEADERS_TABLE::PATH, new QStandardItem(line.mid(start, i+1 - start)));
            QColor logger_text_color = QColor(57, 133, 116);
            model->setData(model->index(row, HEADERS_TABLE::PATH), logger_text_color, Qt::TextColorRole);
            break;
        }
        i++;
    }
    model->setItem(row, HEADERS_TABLE::LOG, new QStandardItem(line.right(line.length() - ++i)));
}

void Table::checkAndSaveFilterValue(QString filter_name, QString filter_value)
{
//    LOG_AUTO_TRACE(logger_ptr, "Table::checkAndSaveFilterValue");
    if (filter_name == "Level") {
        if(std::find(level_filter_value.begin(), level_filter_value.end(), filter_value.toStdString()) != level_filter_value.end()) {

        } else {
//            LOG_DEBUG(logger_ptr, "Added new element to level_filter_value", filter_value.toStdString());
            level_filter_value.push_back(filter_value.toStdString());
        }
    } else if (filter_name == "Thread") {
        if(std::find(thread_filter_value.begin(), thread_filter_value.end(), filter_value.toStdString()) != thread_filter_value.end()) {

        } else {
//            LOG_DEBUG(logger_ptr, "Added new element to thread_filter_value", filter_value.toStdString());
            thread_filter_value.push_back(filter_value.toStdString());
        }
    } else if (filter_name == "Logger Name") {
        if(std::find(logger_name_filter_value.begin(), logger_name_filter_value.end(), filter_value.toStdString()) != logger_name_filter_value.end()) {

        } else {
//            LOG_DEBUG(logger_ptr, "Added new element to logger_name_filter_value", filter_value.toStdString());
            logger_name_filter_value.push_back(filter_value.toStdString());
        }
    } else if (filter_name == "Json") {
        if(std::find(level_filter_value.begin(), level_filter_value.end(), filter_value.toStdString()) != level_filter_value.end()) {

        } else {
//            LOG_DEBUG(logger_ptr, "Added new element to level_filter_value", filter_value.toStdString());
            level_filter_value.push_back(filter_value.toStdString());
        }
    }
}

void Table::resizeColumnsToContents()
{
    LOG_AUTO_TRACE(logger_ptr, "Table::resizeColumnsToContents");
    table_->resizeColumnToContents(HEADERS_TABLE::DATE_TIME);
    table_->resizeColumnToContents(HEADERS_TABLE::THREAD);
    table_->resizeColumnToContents(HEADERS_TABLE::LOGGER_NAME);
    table_->resizeColumnToContents(HEADERS_TABLE::PATH);
    table_->resizeColumnToContents(HEADERS_TABLE::LOG);
}
