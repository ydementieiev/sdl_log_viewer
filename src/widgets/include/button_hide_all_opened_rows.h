#ifndef BUTTON_HIDE_ALL_OPENED_ROWS_H
#define BUTTON_HIDE_ALL_OPENED_ROWS_H

#include <QPushButton>

class Button_hideAllOpenedRows {
public:
    Button_hideAllOpenedRows(QPushButton *hide_all);
    void button_visibility(bool visible);

private:
    QPushButton *hide_all_;

};

#endif // BUTTON_HIDE_ALL_OPENED_ROWS_H
