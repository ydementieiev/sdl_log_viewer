#ifndef FILTER_TREE_H
#define FILTER_TREE_H

#include <QTreeWidget>
#include <QString>
#include <QCheckBox>

#include <vector>
#include <string>

class FilterTree {
public:
    FilterTree();
    FilterTree(QTreeWidget *filter_tree, QLineEdit *serch_input_field);

    void createFilter(QString filter_name, std::vector<std::string> filter_value);
    void find_filter(const QString &new_filter);
    void checkUncheckAll();
    void clearTree();
    std::vector<QString> getActiveFilter();

private:
    void showAllBranch();

    QList<QCheckBox*> m_checkboxes;
    std::vector<QString> active_checkboxes_;
    bool is_all_checked_ = true;
};

#endif // FILTER_TREE_H
