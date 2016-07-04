//analyzer.cpp
#include "analyzer.h"

Analyzer::Analyzer(QObject *parent) :
    QThread(parent)
{
    lAnalyzer = new LexicalAnalyzer;
    sAnalyzer = new SyntaxAnalyzer;
//    evaluator = new Evaluator;

    connect(lAnalyzer, SIGNAL(infoMsg(QString,QColor)), this, SIGNAL(infoMsg(QString,QColor)));
    connect(lAnalyzer, SIGNAL(errorMsg(int,QString,QColor)), this, SIGNAL(errorMsg(int,QString,QColor)));
    connect(lAnalyzer, SIGNAL(warningMsg(int,QString,QColor)), this, SIGNAL(warningMsg(int,QString,QColor)));

    connect(sAnalyzer, SIGNAL(infoMsg(QString,QColor)), this, SIGNAL(infoMsg(QString,QColor)));
    connect(sAnalyzer, SIGNAL(errorMsg(int,QString,QColor)), this, SIGNAL(errorMsg(int,QString,QColor)));

    succed = true;
}

void Analyzer::run()
{
    analyze();

    //exec();
}

void Analyzer::setSrcCode(QString code)
{
    this->code = code;
}

void Analyzer::analyze()
{
    succed = true;

    lAnalyzer->setData(code);
    lAnalyzer->analyze();
    if (!lAnalyzer->isSuccessful()) {
        succed = false;
        return;
    }

    QList<Lexeme> oTable;
    QStringList vTable;
    QStringList cTable;
    QStringList lTable;
    lAnalyzer->getData(&oTable, &vTable, &cTable, &lTable);
    //для цикла
    vTable.push_back("#r1");
    vTable.push_back("#r2");
    m_vTable.clear();
    m_vTable = vTable;
    //
    oTable.push_back(Lexeme("#", -1, -1, -1));

    if (!sAnalyzer->buildTable()) {
        succed = false;
        return;
    }

    sAnalyzer->setLexemesTable(oTable);
    if (!sAnalyzer->analyze()) {
        succed = false;
        return;
    }
}

QList<string> Analyzer::getPolis()
{
    return sAnalyzer->getPolis();
}

QStringList Analyzer::getVTable()
{
    return m_vTable;
}

QList<Lexeme> Analyzer::getOTbale()
{
    QList<Lexeme> oTable;
    QStringList vTable;
    QStringList cTable;
    QStringList lTable;
    lAnalyzer->getData(&oTable, &vTable, &cTable, &lTable);
    return oTable;
}

int** Analyzer::getPrecTable()
{
    return sAnalyzer->getPrecTable();
}

int Analyzer::getGrammarSize()
{
    return sAnalyzer->getGrammarSize();
}

vector<string> Analyzer::getGrammarSymbols()
{
    return sAnalyzer->getGrammarSymbols();
}

QList<QStringList> Analyzer::getAnalyzeProcess()
{
    return sAnalyzer->getAnalyzeProcess();
}

QList<QStringList> Analyzer::getAnalyzePolisProcess()
{
    return sAnalyzer->getAnalyzePolisProcess();
}
