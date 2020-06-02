#include "filter_tree.h"

#include <QCheckBox>
#include <QDebug>

#include "logger.h"

CREATE_LOGGERPTR_GLOBAL(logger_ptr, "FilterTree", "filter_tree.cpp");

static QTreeWidget *filter_tree_;
static QLineEdit *serch_input_field_;

FilterTree::FilterTree()
{
}

FilterTree::FilterTree(QTreeWidget *filter_tree, QLineEdit *serch_input_field)
{
    filter_tree_ = filter_tree;
    serch_input_field_ = serch_input_field;
}

void FilterTree::createFilter(QString filter_name, std::vector<std::string> filter_value)
{
    LOG_AUTO_TRACE(logger_ptr, "FilterTree::createFilter");

    QTreeWidgetItem *topLevelItem = new QTreeWidgetItem(filter_tree_);
    filter_tree_->addTopLevelItem(topLevelItem);
    topLevelItem->setText(0, filter_name);
    for (unsigned long i = 0; i < filter_value.size(); ++i) {
        QString qstr = QString::fromStdString(filter_value[i]);
        QCheckBox* checkbox = new QCheckBox(qstr);
        checkbox->setChecked(true);
        m_checkboxes.append(checkbox);
        filter_tree_->setItemWidget(new QTreeWidgetItem(topLevelItem), 0, checkbox);
    }
}

void FilterTree::find_filter(const QString &new_filter)
{
    LOG_AUTO_TRACE(logger_ptr, "FilterTree::find_filter");

    showAllBranch();
    for (int i = 0; i < m_checkboxes.size(); i++) {
        m_checkboxes[i]->setStyleSheet("");
        if (!new_filter.isEmpty() && m_checkboxes[i]->text().contains(new_filter, Qt::CaseInsensitive)) {
            m_checkboxes[i]->setStyleSheet("border: 0.5px solid; border-color: rgb(193, 125, 17);");
        }
    }
}

void FilterTree::checkUncheckAll()
{
    LOG_AUTO_TRACE(logger_ptr, "FilterTree::checkUncheckAll");

    is_all_checked_ = !is_all_checked_;
    showAllBranch();
    for (int i = 0; i < m_checkboxes.size(); i++) {
        m_checkboxes[i]->setChecked(is_all_checked_);
    }
}

void FilterTree::clearTree()
{
    LOG_AUTO_TRACE(logger_ptr, "FilterTree::clearTree");

    filter_tree_->clear();
}

std::vector<QString> FilterTree::getActiveFilter()
{
    LOG_AUTO_TRACE(logger_ptr, "FilterTree::getActiveFilter");

    active_checkboxes_.clear();
    for (int i = 0; i < m_checkboxes.size(); i++) {
        if (!m_checkboxes[i]->isChecked()) {
            active_checkboxes_.push_back(m_checkboxes[i]->text());
        }
    }
    return active_checkboxes_;
}

void FilterTree::showAllBranch()
{
    LOG_AUTO_TRACE(logger_ptr, "FilterTree::showAllBranch");

    int topCount = filter_tree_->topLevelItemCount();
    for (int i = 0; i < topCount; i++) {
        QTreeWidgetItem *item = filter_tree_->topLevelItem(i);
        item->setExpanded(true);
    }
}
