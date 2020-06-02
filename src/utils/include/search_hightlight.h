#ifndef SEARCH_HIGHTLIGHT_H
#define SEARCH_HIGHTLIGHT_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>

class SearchHighLight : public QSyntaxHighlighter
{
    Q_OBJECT
    using BaseClass = QSyntaxHighlighter;
public:
    explicit SearchHighLight(QTextDocument* parent = nullptr);

    void searchText(const QString& text);

protected:
    virtual void highlightBlock(const QString &text) override;

private:
    QRegularExpression m_pattern; // Регулярное выражение для поиска, в нашем случае это слово или текст
    QTextCharFormat m_format;     // Форматирование текста, подсветка
};

#endif // SEARCH_HIGHTLIGHT_H
