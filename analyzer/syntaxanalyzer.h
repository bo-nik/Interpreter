#ifndef SYNTAXANALYZER_H
#define SYNTAXANALYZER_H

#include <QFile>
#include <QTextStream>
#include <QColor>

#include "analyzer/precedencetablebuilder.h"
#include "analyzer/lexicalanalyzer.h"

#define PRIORITY_TABLE_FILE ":/data/priority.table"

struct priorityItem
{
    priorityItem(string lexeme, int p) : lex(lexeme), priority(p) {}
    string lex;
    int priority;
};

class SyntaxAnalyzer : public PrecedenceTableBuilder
{
public:
    SyntaxAnalyzer();

    bool analyze();
    void setLexemesTable(QList<Lexeme> oTable);
    QList<string> getPolis();
    QList<QStringList> getAnalyzeProcess();
    QList<QStringList> getAnalyzePolisProcess();

private:
    int getRalation(string symbol1, string symbol2);//получить отношение между 2мя символамми
    string checkRule(QList<string> rule, string prev1, string prev2);//проверка наличия правила в грамматике
    bool readPriorityTable();
    int getSymbolPriority(string symbol);

private:
    QList<Lexeme> m_oTable;
    QList<priorityItem> priorityTable;
    QList<priorityItem> polisOut;

    QList<QStringList> analyseProcess;
    QList<QStringList> analysePolisProcess;
};

#endif // SYNTAXANALYZER_H
