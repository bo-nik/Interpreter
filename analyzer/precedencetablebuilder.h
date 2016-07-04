#ifndef PRECEDENCETABLEBUILDER_H
#define PRECEDENCETABLEBUILDER_H

#include <QObject>
#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <QFile>
#include <QTextStream>
#include <QColor>
#include <QTime>

#include "analyzer/grammar.h"

using namespace std;

#define GRAMMAR_FILE        ":/data/grammar.info"
#define CSV_SEPARATOR       ";"
#define GRAMMAR_ASSIGMENT   "::="
#define RULE_STOP_SYMBOL    "!"

//значения для таблицы
#define T_DEFAULT           0 //
#define T_EQUAL             1 // =
#define T_MORE              2 // >
#define T_LESS              3 // <

class PrecedenceTableBuilder : public QObject
{
    Q_OBJECT
public:
    explicit PrecedenceTableBuilder(QObject *parent = 0);
    ~PrecedenceTableBuilder();

    bool buildTable();
    int** getPrecTable();
    int getGrammarSize();
    vector<string> getGrammarSymbols();

private:
    bool readGrammar();//прочитать грамматику из файла
    bool fillTable();//заполнить таблицу предшествований
    bool isTeriminal(string symbol);//терминальный ли символ
    void getFirst(string symbol, vector<string> *first_symbols);//получить FIRST+
    void getLast(string symbol, vector<string> *last_symbols);//получить LAST+
    bool putToTable(string symbol1, string symbol2, int relations);//поставить символ в таблицу

protected:
    int getGrammarSymbolNumber(string symbol);//получить номер символа грамматики
    char getRalationsSymbol(int relations);//получить символ отношения по коду

protected:
    vector<string> grammar_symbols;//символы грамматики
    vector<vector<string> > grammar;//грамматика построчно
    int **g_table;//таблица предшесвований

signals:
    void infoMsg(QString msg, QColor color);
    void errorMsg(int line, QString msg, QColor color);
};

#endif // PRECEDENCETABLEBUILDER_H
