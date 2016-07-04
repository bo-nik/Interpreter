#include "highlighter.h"
#include "iostream"

Highlighter::Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkCyan);
    //keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\bprogram\\b" << "\\bvar\\b" << "\\bbegin\\b"
            << "\\bend\\b" << "\\bfloat\\b" << "\\bif\\b"
            << "\\bthen\\b" << "\\bgoto\\b" << "\\bdo\\b"
            << "\\bto\\b" << "\\bnext\\b";
    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    constantFormat.setForeground(Qt::blue);
    constExpression = QRegExp("\\.?((\\d+\\.\\d*)|(\\d+))");
    rule.pattern = constExpression;
    rule.format = constantFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\b[a-z][a-z0-9_]{0,}(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    labelFormat.setFontItalic(true);
    //labelFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegExp("\\b[a-z][a-z0-9_]{0,}(?=\\:)");
    rule.format = labelFormat;
    highlightingRules.append(rule);
}

void Highlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules)
    {
        QRegExp expression(rule.pattern);
        int index = expression.indexIn(text);
        while (index >= 0)
        {
            int length = expression.matchedLength();
            setFormat(index, length, rule.format);
            index = expression.indexIn(text, index + length);
        }
    }
    setCurrentBlockState(0);

    //поиск констант
//    int index = constExpression.indexIn(text);
//    if(index > 0 && ((text[index-1] >= 97 && text[index-1] <= 122) || text[index-1] == '_'))
//        if(text[index] != '+' && text[index] != '-')
//            index = -1;
//    while (index >= 0)
//    {
//        int length = constExpression.matchedLength();
//        if(!(index > 0 && ((text[index-1] >= 97 && text[index-1] <= 122) || text[index-1] == '_')))
//            setFormat(index, length, constantFormat);
//        index = constExpression.indexIn(text, index + length);
//    }
}
