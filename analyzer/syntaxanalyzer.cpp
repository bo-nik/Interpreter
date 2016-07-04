#include "syntaxanalyzer.h"

#include <iostream>
using namespace std;

SyntaxAnalyzer::SyntaxAnalyzer()
{
}

bool SyntaxAnalyzer::analyze()
{
    emit infoMsg("Синтаксический разбор начат в " +
                 QTime::currentTime().toString() + " " +
                 QDate::currentDate().toString(Qt::DefaultLocaleShortDate),
                 Qt::black);

    readPriorityTable();//читаем таблицу с проритетами
    //для потроения полиза
    QList<priorityItem> polisStack;
    //QList<priorityItem> polisOut;
    polisOut.clear();
    bool buildPolis = false;
    analyseProcess.clear();
    analysePolisProcess.clear();

    QList<string> stack;
    QList<string> input;
    bool loop = false;//признак цикла
    string loopParam;//параметр цикла
    int rwcount = 0;//кол-во переменных для read/write
    bool rwcountFlag = false;//считать ли кол-во переменных (если встретили read/write - тогда true)
    int labelsNumbers = 1;//кол-во использованных меток
    for (unsigned int i = 0; i < m_oTable.size(); i++) {
        if (m_oTable[i].lex == "\n") {
            input.push_back("~");
        }
        else if (m_oTable[i].n == ID_NUMBER) {
            input.push_back("id");
        }
        else if (m_oTable[i].n == CONST_NUMBER) {
            input.push_back("const");
        }
        else if (m_oTable[i].n == LABEL_NUMBER) {
            input.push_back("label");
        }
        else {
            input.push_back(m_oTable[i].lex.toStdString());
        }
    }
    stack.push_back("#");

    int rel = -1;
    int line = 1;
    while (true) {
        QStringList iterationItem;
        QString ts;
        for (unsigned int i = 0; i < stack.size(); i++) {
            ts += stack[i].c_str() + QString(" ");
        }
        iterationItem << ts;

        //получаем отношение между 2мя символами входной цепочки
        rel = getRalation(stack[stack.size()-1], input[0]);

        iterationItem << QString(getRalationsSymbol(rel));
        ts = "";
        for (unsigned int i = 0; i < input.size(); i++) {
            ts += input[i].c_str() + QString(" ");
        }
        iterationItem << ts;

        analyseProcess.push_back(iterationItem);
        iterationItem.clear();
        ts = "";

        //проверка условия окончания
        if (stack.size() == 2 && stack[1] == grammar[0][0]) {
            //cout << "Синтаксический разбор закончен успешно..." << endl;
            emit infoMsg("Синтаксический разбор закончен УСПЕШНО в " +
                         QTime::currentTime().toString() + " " +
                         QDate::currentDate().toString(Qt::DefaultLocaleShortDate),
                         Qt::black);
            //для полиза
            //переписываем все из стека на выход
            while (polisStack.size()) {
                if (polisStack[polisStack.size()-1].lex == "(") {
                    polisStack.pop_back();
                    continue;
                }
                polisOut.push_back(polisStack[polisStack.size()-1]);
                polisStack.pop_back();
            }

            return true;
        }
        //

        if (rel == T_LESS || rel == T_EQUAL) {
            stack.push_back(input[0]);
            input.erase(input.begin());

            //проверяем начало новой строки
            if (input[0] == "~") {
                line++;
            }

            ////////////////////////////////////////////////////////////////////////////////////////////
            //Строим полиз                                                                            //
            ////////////////////////////////////////////////////////////////////////////////////////////
            if (m_oTable[m_oTable.size() - input.size()].lex.toStdString() == "end") {
                buildPolis = false;
            }

            if (buildPolis) {
                int outCurPos = polisOut.size();
                int n = m_oTable[m_oTable.size() - input.size()].n;
                string lex = m_oTable[m_oTable.size() - input.size()].lex.toStdString();
                //заменяем унарный минус на @ и \n на ~
                if (lex == "-" &&  m_oTable[m_oTable.size() - input.size()-1].n != ID_NUMBER &&
                        m_oTable[m_oTable.size() - input.size()-1].n != CONST_NUMBER) {
                    lex = "@";
                }
                if (lex == "\n") {
                    lex = "~";
                }
                int priority = getSymbolPriority(lex);
                //                polisFile << "\"" << lex << "\"" << CSV_SEPARATOR;
                iterationItem << lex.c_str();

                if (n == ID_NUMBER || n == CONST_NUMBER/* || n == LABEL_NUMBER*/) {
                    polisOut.push_back(priorityItem(lex, priority));
                    if (rwcountFlag) {
                        rwcount++;
                    }
                }
                if (n == LABEL_NUMBER && m_oTable[m_oTable.size() - input.size() - 1].lex.toStdString() != "goto") {
                    polisOut.push_back(priorityItem(lex + ":", priority));
                }
                if (n == LABEL_NUMBER && m_oTable[m_oTable.size() - input.size() - 1].lex.toStdString() == "goto") {
                    polisOut.push_back(priorityItem("not", getSymbolPriority("not")));
                    polisOut.push_back(priorityItem(lex, priority));
                    polisOut.push_back(priorityItem("JMPF", -1));
                }

                if (priority != -1) {
                    while (true) {
                        if (lex == "if") {
                            polisStack.push_back(priorityItem(lex, getSymbolPriority(lex)));
                            break;
                        }
                        if (lex == "do") {
                            polisStack.push_back(priorityItem(lex +
                                                              QString(" #m").toStdString() + QString::number(labelsNumbers).toStdString()  +
                                                              QString(" #m").toStdString()   + QString::number(labelsNumbers + 1).toStdString()  +
                                                              QString(" #m").toStdString()  + QString::number(labelsNumbers + 2).toStdString() ,
                                                              priority));
                            labelsNumbers += 3;
                            loop = true;
                            break;
                        }
                        if (lex == "=" && loop) {
                            loopParam = polisOut[polisOut.size() - 1].lex;
                        }
                        if (lex == "read" || lex == "write") {
                            rwcountFlag = true;
                            rwcount = 0;
                        }
                        //если стек пустой, пишем операцию в стек
                        if (polisStack.size() == 0) {
                            if (lex == "~" || lex == "then") {
                                break;
                            }
                            polisStack.push_back(priorityItem(lex, priority));
                            break;
                        }
                        else {
                            int priorityStack = getSymbolPriority(polisStack[polisStack.size()-1].lex);//приоритет последней операции в стеке
                            if (priority <= priorityStack) {
                                if (lex != "(" && lex != "[") {
                                    if (polisStack[polisStack.size() - 1].lex == "read" ||
                                            polisStack[polisStack.size() - 1].lex == "write") {
                                        polisOut.push_back(priorityItem(QString::number(rwcount).toStdString(), -1));
                                        rwcountFlag = false;
                                        rwcount = 0;
                                    }
                                    polisOut.push_back(polisStack[polisStack.size()-1]);
                                    polisStack.pop_back();
                                    continue;
                                }
                                else {
                                    if (lex == "~" || lex == "then" || lex == "to") {
                                        break;
                                    }
                                    polisStack.push_back(priorityItem(lex, priority));
                                    break;
                                }
                            }
                            //закрывающая скобка должна выталкивать открывающюю
                            else if (lex == ")" && polisStack[polisStack.size()-1].lex == "(") {
                                polisStack.pop_back();
                                break;
                            }
                            else if (lex == "]" && polisStack[polisStack.size()-1].lex == "[") {
                                polisStack.pop_back();
                                break;
                            }
                            else {
                                if (lex != ")") {
                                    if (lex == "~" || lex == "then") {
                                        if (lex == "~" && polisStack[polisStack.size() - 1].lex == "if") {
                                            polisStack.pop_back();
                                        }
                                        if (lex == "~" && loop) {
                                            //ищем ближайшую запись do в стеке и вытаскиваем инема меток
                                            QStringList labels;
                                            for (unsigned int i = polisStack.size() - 1; i >= 0; i--) {
                                                if (polisStack[i].lex[0] == 'd' && polisStack[i].lex[1] == 'o') {
                                                    labels = QString(polisStack[i].lex.c_str()).split(" ");
                                                    break;
                                                }
                                            }

                                            polisOut.push_back(priorityItem("=", -1));
                                            polisOut.push_back(priorityItem("#r1", -1));
                                            polisOut.push_back(priorityItem("0", -1));
                                            polisOut.push_back(priorityItem("==", -1));
                                            polisOut.push_back(priorityItem(labels[2].toStdString(), -1));
                                            polisOut.push_back(priorityItem("JMPF", -1));
                                            polisOut.push_back(priorityItem(loopParam, -1));
                                            polisOut.push_back(priorityItem(loopParam, -1));
                                            polisOut.push_back(priorityItem("1", -1));
                                            polisOut.push_back(priorityItem("+", -1));
                                            polisOut.push_back(priorityItem("=", -1));
                                            polisOut.push_back(priorityItem(labels[2].toStdString() + ":", -1));
                                            polisOut.push_back(priorityItem("#r1", -1));
                                            polisOut.push_back(priorityItem("0", -1));
                                            polisOut.push_back(priorityItem("=", -1));
                                            polisOut.push_back(priorityItem(loopParam, -1));
                                            polisOut.push_back(priorityItem("#r2", -1));
                                            polisOut.push_back(priorityItem("-", -1));
                                            polisOut.push_back(priorityItem("0", -1));
                                            polisOut.push_back(priorityItem("<=", -1));
                                            polisOut.push_back(priorityItem(labels[3].toStdString(), -1));
                                            polisOut.push_back(priorityItem("JMPF", -1));

                                            loop = false;
                                        }
                                        break;
                                    }
                                    if (lex == "to") {
                                        //ищем ближайшую запись do в стеке и вытаскиваем инема меток
                                        QStringList labels;
                                        for (unsigned int i = polisStack.size() - 1; i >= 0; i--) {
                                            if (polisStack[i].lex[0] == 'd' && polisStack[i].lex[1] == 'o') {
                                                labels = QString(polisStack[i].lex.c_str()).split(" ");
                                                break;
                                            }
                                        }
                                        polisOut.push_back(priorityItem("#r1", -1));
                                        polisOut.push_back(priorityItem("1", -1));
                                        polisOut.push_back(priorityItem("=",  getSymbolPriority("=")));
                                        polisOut.push_back(priorityItem(labels[1].toStdString() + ":", -1));
                                        polisOut.push_back(priorityItem("#r2", -1));
                                        break;
                                    }
                                    if (lex == "next") {
                                        //ищем ближайшую запись do в стеке и вытаскиваем инема меток
                                        QStringList labels;
                                        for (unsigned int i = polisStack.size() - 1; i >= 0; i--) {
                                            if (polisStack[i].lex[0] == 'd' && polisStack[i].lex[1] == 'o') {
                                                labels = QString(polisStack[i].lex.c_str()).split(" ");
                                                break;
                                            }
                                        }
                                        polisOut.push_back(priorityItem(labels[1].toStdString(), -1));
                                        polisOut.push_back(priorityItem("JMP", -1));
                                        polisOut.push_back(priorityItem(labels[3].toStdString() + ":", -1));

                                        polisStack.pop_back();
                                        break;
                                    }
                                    polisStack.push_back(priorityItem(lex, priority));
                                    break;
                                }
                                else {
                                    break;
                                }
                            }
                        }
                    }
                }

                //                polisFile << "\"";
                //                for (unsigned int i = 0; i < polisStack.size(); i++) {
                //                    polisFile << polisStack[i].lex << " ";
                //                }
                //                polisFile << "\"";
                //                polisFile << CSV_SEPARATOR;
                for (unsigned int i = 0; i < polisStack.size(); i++) {
                    ts += polisStack[i].lex.c_str() + QString(" ");
                }
                iterationItem << ts;
                ts = "";
                for (unsigned int i = outCurPos; i < polisOut.size(); i++) {
                    ts += polisOut[i].lex.c_str() + QString(" ");
                }
                iterationItem << ts;
                analysePolisProcess.push_back(iterationItem);

                //                polisFile << "\"";
                //                for (unsigned int i = outCurPos; i < polisOut.size(); i++) {
                //                    polisFile << polisOut[i].lex << " ";
                //                }
                //                polisFile << "\"";

                //                polisFile << endl;
            }
            //начинем строить полиз только после начала блока кода
            if (m_oTable[m_oTable.size() - input.size() - 1].lex.toStdString() == "begin") {
                buildPolis = true;
            }
            ////////////////////////////////////////////////////////////////////////////////////////////
        }
        else if (rel == T_MORE) {
            int pos = stack.size()-1;
            while (true) {
                int rel1 = getRalation(stack[pos-1], stack[pos]);
                if (rel1 == T_MORE || rel1 == T_EQUAL) {
                    pos--;
                    if (pos < 1) {
                        //cout << "Синтаксический разбор закончен с ошибками... Строка " <<  line << endl;
                        emit errorMsg(line, "Синтаксическая ошибка", Qt::black);
                        emit infoMsg("Синтаксический разбор закончен с ОШИБКАМИ в " +
                                     QTime::currentTime().toString() + " " +
                                     QDate::currentDate().toString(Qt::DefaultLocaleShortDate),
                                     Qt::darkRed);
                        //                        fileo.close();
                        //                        polisFile.close();
                        return false;
                    }
                }
                else if (rel1 == T_LESS) {
                    QList<string> rule;
                    for (unsigned int i = pos; i < stack.size(); i++) {
                        rule.push_back(stack[i]);
                    }
                    //hack
                    string rule_main;
                    if (pos > 1) {
                        rule_main = checkRule(rule, stack[pos-1], stack[pos-2]);
                    }
                    else {
                        rule_main = checkRule(rule, stack[0], stack[0]);
                    }
                    if (rule_main == "") {
                        //cout << "Синтаксический разбор закончен с ошибками... Строка " <<  line << endl;
                        emit errorMsg(line, "Синтаксическая ошибка", Qt::black);
                        emit infoMsg("Синтаксический разбор закончен с ОШИБКАМИ в " +
                                     QTime::currentTime().toString() + " " +
                                     QDate::currentDate().toString(Qt::DefaultLocaleShortDate),
                                     Qt::darkRed);
                        //                        fileo.close();
                        //                        polisFile.close();
                        return false;
                    }
                    while (stack.size() > pos) {
                        stack.erase(stack.end()-1);
                    }
                    stack.push_back(rule_main);
                    break;
                }
                else {
                    //cout << "Синтаксический разбор закончен с ошибками... Строка " <<  line << endl;
                    emit errorMsg(line, "Синтаксическая ошибка", Qt::black);
                    emit infoMsg("Синтаксический разбор закончен с ОШИБКАМИ в " +
                                 QTime::currentTime().toString() + " " +
                                 QDate::currentDate().toString(Qt::DefaultLocaleShortDate),
                                 Qt::darkRed);
                    //                    fileo.close();
                    //                    polisFile.close();
                    return false;
                }
            }
        }
        else {
            //cout << "Синтаксический разбор закончен с ошибками... Строка " <<  line << endl;
            emit errorMsg(line, "Синтаксическая ошибка", Qt::black);
            emit infoMsg("Синтаксический разбор закончен с ОШИБКАМИ в " +
                         QTime::currentTime().toString() + " " +
                         QDate::currentDate().toString(Qt::DefaultLocaleShortDate),
                         Qt::darkRed);
            //            fileo.close();
            //            polisFile.close();
            return false;
        }
    }
}

void SyntaxAnalyzer::setLexemesTable(QList<Lexeme> oTable)
{
    m_oTable = oTable;
}

//Получить отношение между 2мя символами
int SyntaxAnalyzer::getRalation(string symbol1, string symbol2)
{
    int n1 = getGrammarSymbolNumber(symbol1);
    int n2 = getGrammarSymbolNumber(symbol2);
    if (n1 >= grammar_symbols.size() || n2 >= grammar_symbols.size()) {
        return -1;
    }
    return g_table[n1][n2];
}

//проверка наличия правила в грамматике
//string checkRule(QList<string> rule, QList<QList<string> > grammar)
string SyntaxAnalyzer::checkRule(QList<string> rule, string prev1, string prev2)
{
    //hack for <id_list> ::= id и <factor> ::= id
    if (rule.size() == 1 && rule[0] == "id") {
        if (prev1 == "var" || (prev1 == "(" && (prev2 == "read") || prev2 == "write")) {
            return "<id_list>";
        }
        else {
            return "<factor>";
        }
    }
    //

    for (int i = 0; i < grammar.size(); i++) {
        if (rule.size() != grammar[i].size() - 2) {
            continue;
        }
        bool ok = true;
        for (int j = 0; j < rule.size(); j++) {
            if (rule[j] != grammar[i][j+2]) {
                ok = false;
                break;
            }
        }
        if (ok) {
            return grammar[i][0];
        }
    }
    return "";
}

bool SyntaxAnalyzer::readPriorityTable()
{
    QFile f(PRIORITY_TABLE_FILE);
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!f.isOpen()) {
        emit infoMsg("Не удалось прочитать таблицу приоритетов", Qt::darkRed);
        return false;
    }
    QTextStream file(&f);

    priorityTable.clear();

    //читаем из файла
    while(true) {
        QString qs;
        string s;
        int p;
        file >> qs;
        s = qs.toStdString();
        file >> p;
        if(file.atEnd())
            break;
        priorityTable.push_back(priorityItem(s, p));
    }

    f.close();//закрываем файл

    emit infoMsg("Прочитали таблицу приоритетов...", Qt::black);
    return true;
}

int SyntaxAnalyzer::getSymbolPriority(string symbol)
{
    for (unsigned int i = 0; i < priorityTable.size(); i++) {
        if (priorityTable[i].lex == symbol) {
            return priorityTable[i].priority;
        }
    }
    return -1;
}

QList<string> SyntaxAnalyzer::getPolis()
{
    QList<string> polis;
    for (int i = 0; i < polisOut.size(); i++) {
        polis.push_back(polisOut[i].lex);
    }
    return polis;
}

QList<QStringList> SyntaxAnalyzer::getAnalyzeProcess()
{
    return analyseProcess;
}

QList<QStringList> SyntaxAnalyzer::getAnalyzePolisProcess()
{
    return analysePolisProcess;
}
