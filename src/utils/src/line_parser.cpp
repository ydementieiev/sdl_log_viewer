#include "line_parser.h"
#include <string>

LineParser::LineParser(QString &line_for_parse, bool need_parse)
    :line_(line_for_parse)
{
    if (need_parse) {
        parse_json();
    }

    AddColorToString();
    parse_line_to_qtextEdit();
}

LineParser::LineParser(QString &line_for_parse, const QString &finded_text)
    :line_(line_for_parse)
{
    AddColorToString();
    parse_line_to_qtextEdit();

    addColorToFinded(finded_text);
}

void LineParser::parse_line_to_qtextEdit()
{
    textEdit = new QTextEdit(line_);
    textEdit->setReadOnly(true);
    textEdit->setLineWrapMode(QTextEdit::LineWrapMode::NoWrap);
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    textEdit->setMinimumWidth(static_cast<int>(textEdit->document()->idealWidth()));
    textEdit->setStyleSheet("QTextEdit { border: 0px }");
}

void LineParser::AddColorToString()
{
    QString str;
    bool string_finded = false;
    bool number_finded = false;

    for (int i = 0; i < line_.size(); i++) {
        if (line_[i] == '\"' && !string_finded) {
            str += STRING_FONT_START;
            string_finded = true;
        } else if (line_[i] == '\"' && string_finded) {
            str += line_[i] + FONT_FINISH;
            string_finded = false;
            continue;
        } else if (!string_finded && line_[i].isNumber() && !number_finded) {
            str += BLUE_FONT_START + line_[i];
            number_finded = true;
        } else if (number_finded && !line_[i].isNumber() && !string_finded) {
            str += FONT_FINISH + line_[i];
            number_finded = false;
            continue;
        }
        str += line_[i];
    }
    line_= str;
}


QTextEdit* LineParser::getTextEdit()
{
    return textEdit;
}

void LineParser::addColorToFinded(const QString &finded_text)
{
    qDebug() << "LineParser::addColorToFinded" << '\n';

   SearchHighLight *m_searchHighLight = new SearchHighLight(textEdit->document());
   m_searchHighLight->searchText(finded_text);
}

void LineParser::parse_json()
{

//    utils::JsonReader reader;
//    std::string json = line_.toStdString();
//    Json::Value root;

//    if (!reader.parse(json, &root)) {
////      LOG4CXX_DEBUG(logger_, "Unable to parse query_app json file. ");
//      return;
//    }
//    QString str = root.asCString();
//    qDebug() << str;
}
