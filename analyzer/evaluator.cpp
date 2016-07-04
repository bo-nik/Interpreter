//evaluator.cpp
#include "evaluator.h"

Evaluator::Evaluator(QObject *parent) :
    QThread(parent)
{
    eps = 0.000001;
    idReaded = false;
    idValue = 0;
    succed = true;
}

void Evaluator::run()
{
    succed = evaluate();
}

bool Evaluator::evaluate()
{
    makeLTable();

    //    cout << "Давайте выполним сие чудо..." << endl;
    //    cout << "---------------------" << endl;

    QList<string> stack;
    QList<string> inputCode = input;

    for (unsigned int curPos = 0; curPos < inputCode.size(); curPos++) {
        //если id или const - заносим в стек
        if (isID(inputCode[curPos]) || isConst(inputCode[curPos]) || isLabel(inputCode[curPos])) {
            stack.push_back(inputCode[curPos]);
        }
        //иначе будем что-то делать
        else {
            //2х-месные операции, для которых нужно 2 операнда (константы или переменные)
            if (inputCode[curPos] == "+" || inputCode[curPos] == "-" || inputCode[curPos] == "*" ||
                    inputCode[curPos] == "/" || inputCode[curPos] == "=" || inputCode[curPos] == "<" ||
                    inputCode[curPos] == "<=" || inputCode[curPos] == ">" || inputCode[curPos] == ">=" ||
                    inputCode[curPos] == "==" || inputCode[curPos] == "!=" || inputCode[curPos] == "and" ||
                    inputCode[curPos] == "or") {
                //получаем значения переменных
                double v1 = getValue(stack[stack.size()-2]);
                double v2 = getValue(stack[stack.size()-1]);

                if (inputCode[curPos] == "+") {
                    stack[stack.size()-2] = QString::number(v1 + v2).toStdString();
                }
                else if(inputCode[curPos] == "-") {
                    stack[stack.size()-2] = QString::number(v1 - v2).toStdString();
                }
                else if(inputCode[curPos] == "*") {
                    stack[stack.size()-2] = QString::number(v1 * v2).toStdString();
                }
                else if(inputCode[curPos] == "/") {
                    stack[stack.size()-2] = QString::number(v1 / v2).toStdString();
                }
                else if(inputCode[curPos] == "=") {
                    setVarValue(stack[stack.size()-2], QString::number(v2).toStdString());
                }
                else if(inputCode[curPos] == "<") {
                    stack[stack.size()-2] = QString::number(v1 < v2).toStdString();
                }
                else if(inputCode[curPos] == "<=") {
                    stack[stack.size()-2] = QString::number(v1 <= v2).toStdString();
                }
                else if(inputCode[curPos] == ">") {
                    stack[stack.size()-2] = QString::number(v1 > v2).toStdString();
                }
                else if(inputCode[curPos] == ">=") {
                    stack[stack.size()-2] = QString::number(v1 >= v2).toStdString();
                }
                else if(inputCode[curPos] == "==") {
                    stack[stack.size()-2] = QString::number(/*v1 == v2*/fabs(v1 - v2) <= eps).toStdString();
                }
                else if(inputCode[curPos] == "!=") {
                    stack[stack.size()-2] = QString::number(/*v1 != v2*/fabs(v1 - v2) >= eps).toStdString();
                }
                else if(inputCode[curPos] == "or") {
                    stack[stack.size()-2] = QString::number(v1 || v2).toStdString();
                }
                else if(inputCode[curPos] == "and") {
                    stack[stack.size()-2] = QString::number(v1 && v2).toStdString();
                }
                stack.erase(stack.end()-1);
            }
            //1-месные операции
            else if(inputCode[curPos] == "@" || inputCode[curPos] == "not") {
                double v1 = getValue(stack[stack.size()-1]);
                if (inputCode[curPos] == "@") {
                    stack[stack.size()-1] = QString::number(-v1).toStdString();
                }
                else if (inputCode[curPos] == "not") {
                    stack[stack.size()-1] = QString::number(!v1).toStdString();
                }
            }
            else if (inputCode[curPos] == "read") {
                int wcount = (int)getConstValue(stack[stack.size()-1]);
                for (unsigned int i = wcount; i > 0; i--) {
                    //ждем влаг о окончании ввода
                    idReaded = false;
                    emit needVarValForRead(QString("Введите ") +
                                           QString(stack[stack.size()-i-1].c_str()) +
                                           QString(" > "));
                    while (!idReaded) {
                        msleep(300);
                    }
                    setVarValue(stack[stack.size()-i-1], idValue);
                }
                for (int i = 0; i < wcount + 1; i++) {
                    stack.pop_back();
                }
            }
            else if (inputCode[curPos] == "write") {
                int rcount = (int)getConstValue(stack[stack.size()-1]);
                QString msg;
                for (unsigned int i = rcount; i > 0; i--) {
                    msg += stack[stack.size()-i-1].c_str() + QString(" = ") +
                            QString::number(getVarValue(stack[stack.size()-i-1])) + QString("; ");
                }
                //cout << endl;
                emit programMsg(msg);
                for (int i = 0; i < rcount + 1; i++) {
                    stack.pop_back();
                }
            }
            else if (inputCode[curPos] == "JMPF") {
                double v1 = getValue(stack[stack.size()-2]);
                string label = stack[stack.size()-1];
                if (!v1) {
                    curPos = getLabelPosition(label, curPos);
                }
                stack.pop_back();
                stack.pop_back();
            }
            else if (inputCode[curPos] == "JMP") {
                string label = stack[stack.size()-1];
                curPos = getLabelPosition(label, curPos);
                stack.pop_back();
            }
        }
    }

    //    cout << "---------------------" << endl;
    //    cout << "Ну вот и все..." << endl;
    return true;
}

void Evaluator::addVarible(string var, double val)
{
    for (unsigned int i = 0; i < vTable.size(); i++) {
        if (vTable[i].lex == var) {
            return;
        }
    }
    vTable.push_back(vTableItem(var, val));
}

void Evaluator::clearVTable()
{
    vTable.clear();
}

void Evaluator::setInputCode(QList<string> inCode)
{
    input = inCode;
}

bool Evaluator::isID(string lex)
{
    for (unsigned int i = 0; i < vTable.size(); i++) {
        if (vTable[i].lex == lex) {
            return true;
        }
    }
    return false;
}

bool Evaluator::isConst(string lex)
{
    int dots = 0;
    if ((lex[0] < '0' || lex[0] > '9') && !(lex[0] == '-' && lex.size() > 1)) {
        return false;
    }
    for (unsigned int i = 1; i < lex.size(); i++) {
        if ((lex[i] < '0' || lex[i] > '9') && lex[i] != '.') {
            return false;
        }
        if (lex[i] == '.') {
            dots++;
        }
    }
    if (dots > 1) {
        return false;
    }
    return true;
}

bool Evaluator::isLabel(string lex)
{
    for (unsigned int i = 0; i < lTable.size(); i++) {
        if (lTable[i].lex == lex) {
            return true;
        }
    }
    return false;
}

void Evaluator::makeLTable()
{
    lTable.clear();
    for (unsigned int i = 0; i < input.size(); i++) {
        if (input[i][input[i].size()-1] == ':') {
            lTable.push_back(lTableItem(input[i], i));
            lTable[lTable.size()-1].lex.resize(lTable[lTable.size()-1].lex.size()-1);
        }
    }
}

unsigned int Evaluator::getLabelPosition(string lex, int curPos)
{
    for (unsigned int i = 0; i < lTable.size(); i++) {
        if (lTable[i].lex == lex) {
            return lTable[i].position;
        }
    }
    return curPos;
}

double Evaluator::getVarValue(string lex)
{
    for (unsigned int i = 0; i < vTable.size(); i++) {
        if (vTable[i].lex == lex) {
            return vTable[i].value;
        }
    }
    return 0;
}

double Evaluator::getConstValue(string lex)
{
    char *str = new char [lex.size() + 1];
    for (unsigned int i = 0; i < lex.size(); i++) {
        str[i] = lex[i];
    }
    double res = atof(str);
    delete [] str;
    return res;
}

double Evaluator::getValue(string lex)
{
    if (isID(lex)) {
        return getVarValue(lex);
    }
    else if (isConst(lex)) {
        return getConstValue(lex);
    }
    else {
        return 0;
    }
}

void Evaluator::setVarValue(string var, string val)
{
    if (!isID(var)) {
        return;
    }
    for (unsigned int i = 0; i < vTable.size(); i++) {
        if (vTable[i].lex == var) {
            vTable[i].value = getValue(val);
            break;
        }
    }
}

void Evaluator::setVarValue(string var, double val)
{
    for (unsigned int i = 0; i < vTable.size(); i++) {
        if (vTable[i].lex == var) {
            vTable[i].value = val;
            break;
        }
    }
}

void Evaluator::idForRead(double val)
{
    idValue = val;
    idReaded = true;
}
