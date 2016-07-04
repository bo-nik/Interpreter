//evaluator.h
#ifndef EVALUATOR_H
#define EVALUATOR_H

#include <QThread>
#include <string>
#include <iostream>
#include <QList>
#include <stdlib.h>
#include <QString>
#include <math.h>

using namespace std;

struct vTableItem {
    vTableItem() : value(0) {}
    vTableItem(string s, double val) : lex(s), value(val) {}
    double value;
    string lex;
};

struct lTableItem {
    lTableItem(string s, double pos) : lex(s), position(pos) {}
    double position;
    string lex;
};

class Evaluator : public QThread
{
    Q_OBJECT
public:
    explicit Evaluator(QObject *parent = 0);

    void addVarible(string var, double val = 0);
    void clearVTable();
    void setInputCode(QList<string> inCode);
    bool isSuccessful() {return succed;}

protected:
    void run();

signals:
    void programMsg(QString msg);
    void needVarValForRead(QString promt);

private slots:
    void idForRead(double val);

private:
    bool isID(string lex);
    bool isConst(string lex);
    bool isLabel(string lex);
    void makeLTable();
    double getVarValue(string lex);
    double getConstValue(string lex);
    double getValue(string lex);
    unsigned int getLabelPosition(string lex, int curPos);
    void setVarValue(string var, string val);
    void setVarValue(string var, double val);
    bool evaluate();

private:
    QList<vTableItem> vTable;
    QList<lTableItem> lTable;
    QList<string> input;
    double eps;//для сравнения double
    bool idReaded;
    double idValue;

    bool succed;
};

#endif // EVALUATOR_H
