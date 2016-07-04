#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QVector>
#include <QTextCharFormat>
#include <QRegExp>

class Highlighter : public QSyntaxHighlighter
{
public:
    Highlighter(QTextDocument *parent = 0);

protected:
     void highlightBlock(const QString &text);

private:
    struct HighlightingRule
    {
        QRegExp pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat constantFormat;
    QTextCharFormat functionFormat;
    QTextCharFormat labelFormat;

    QRegExp constExpression;
};

#endif // HIGHLIGHTER_H
