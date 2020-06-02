#ifndef MOCK_LINE_PARSER_H
#define MOCK_LINE_PARSER_H

#include <gmock/gmock.h>
#include "line_parser.h"

class Mock_LineParser : public LineParser
{
public:
    Mock_LineParser(QString &line_for_parse)
    :LineParser(line_for_parse){}

    virtual ~Mock_LineParser() {}

    MOCK_METHOD0(parse_line_to_qtextEdit, void());
    MOCK_METHOD0(AddColorToString, void());
    MOCK_METHOD0(getTextEdit, QTextEdit*());
};

#endif //MOCK_LINE_PARSER_H
