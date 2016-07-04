//analyzer.h
#ifndef ANALYZER_H
#define ANALYZER_H

#include <QThread>
#include <QResource>
#include <string>
#include <QList>

#include "analyzer/lexicalanalyzer.h"
#include "analyzer/syntaxanalyzer.h"

class Analyzer : public QThread
{
    Q_OBJECT
public:
    explicit Analyzer(QObject *parent = 0);
    bool isSuccessful() {return succed;}
    
protected:
    void run();

private:
    void analyze();

public:
    void setSrcCode(QString code);
    QList<string> getPolis();
    QList<Lexeme> getOTbale();
    int** getPrecTable();
    int getGrammarSize();
    vector<string> getGrammarSymbols();
    QList<QStringList> getAnalyzeProcess();
    QList<QStringList> getAnalyzePolisProcess();
    QStringList getVTable();

signals:
    void infoMsg(QString msg, QColor color);
    void errorMsg(int line, QString msg, QColor color);
    void warningMsg(int line, QString msg, QColor color);

private:
    LexicalAnalyzer *lAnalyzer;
    SyntaxAnalyzer *sAnalyzer;

private:
    QString code;
    QStringList m_vTable;
    bool succed;
};

#endif // ANALYZER_H
