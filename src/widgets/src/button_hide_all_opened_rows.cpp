#include "button_hide_all_opened_rows.h"
#include "logger.h"

CREATE_LOGGERPTR_GLOBAL(logger_ptr, "Button_hideAllOpenedRows", "button_hide_all_opened_rows.cpp");

Button_hideAllOpenedRows::Button_hideAllOpenedRows(QPushButton *hide_all)
    : hide_all_(hide_all)
{
    hide_all_->setStyleSheet(QString::fromUtf8("background-color: rgb(30, 30, 30);\n"
"color: rgb(155, 144, 146);"));
    hide_all_->setCheckable(false);
    hide_all_->setAutoExclusive(false);
    hide_all_->hide();
}

void Button_hideAllOpenedRows::button_visibility(bool visible)
{
    LOG_AUTO_TRACE(logger_ptr, "Button_hideAllOpenedRows::button_visibility");
    LOG_DEBUG(logger_ptr, (visible ? "Show" : "Hide"), " all opened rows");
    if (visible) {
        hide_all_->show();
    } else {
        hide_all_->hide();
    }
}
