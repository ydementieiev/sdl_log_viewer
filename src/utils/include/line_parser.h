#ifndef LINE_PARSER_H
#define LINE_PARSER_H

#include <QString>
#include <QDebug>
#include <QTextEdit>
#include "search_hightlight.h"

const QString FONT_FINISH = "</font>";
const QString FONT_FINDED_FINISH = "</font></strong>";
const QString RED_FONT_START = "<strong><font color=red>";
const QString BLUE_FONT_START = "<font color=SteelBlue>";
const QString STRING_FONT_START = "<font color=Sienna>";


class LineParser {
public:
    LineParser(QString &line_for_parse, bool need_parse = false);
    LineParser(QString &line_for_parse, const QString &finded_text);

    QTextEdit* getTextEdit();
private:
    void AddColorToString();
    void parse_line_to_qtextEdit();
    void addColorToFinded(const QString &finded_text);
    void parse_json();
    QString line_;
    QTextEdit *textEdit;
};

#endif // LINE_PARSER_H
