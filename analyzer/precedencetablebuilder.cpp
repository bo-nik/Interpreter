#include "precedencetablebuilder.h"

PrecedenceTableBuilder::PrecedenceTableBuilder(QObject *parent) :
        QObject(parent)
{
    g_table = NULL;
}

PrecedenceTableBuilder::~PrecedenceTableBuilder()
{
    if (g_table != NULL) {
        int table_size = grammar_symbols.size();//кол-во символов в грамматике
        for(int i = 0; i < table_size; i++)
            delete [] g_table[i];
        delete [] g_table;
    }
}

bool PrecedenceTableBuilder::buildTable()
{
    infoMsg("Построение таблици предшествований начато в " +
            QTime::currentTime().toString() + " " +
            QDate::currentDate().toString(Qt::DefaultLocaleShortDate),
            Qt::black);

    grammar_symbols.clear();
    grammar.clear();

    //читаем грамматику с файла
    if (!readGrammar()) {
        infoMsg("Построение таблици окончено с ОШИБКАМИ в " +
                QTime::currentTime().toString() + " " +
                QDate::currentDate().toString(Qt::DefaultLocaleShortDate),
                Qt::darkRed);
        return false;
    }

    int table_size = grammar_symbols.size();//кол-во символов в грамматике
    //объявляем и выделяем память под таблицу предшествований
    if (g_table != NULL) {
        int table_size = grammar_symbols.size();//кол-во символов в грамматике
        for(int i = 0; i < table_size; i++)
            delete [] g_table[i];
        delete [] g_table;
    }
    g_table = new int *[table_size];
    for(int i = 0; i < table_size; i++)
        g_table[i] = new int [table_size];
    //заполняем ее зачениями по умолчанию
    for (int i = 0; i < table_size; i++)
        for (int j = 0; j < table_size; j++)
            g_table[i][j] = T_DEFAULT;

    //заполняем таблиуц
    if (!fillTable()) {
        infoMsg("Построение таблици окончено с ОШИБКАМИ в " +
                QTime::currentTime().toString() + " " +
                QDate::currentDate().toString(Qt::DefaultLocaleShortDate),
                Qt::darkRed);
        return false;
    }

    infoMsg("Построение таблици окончено УСПЕШНО в " +
            QTime::currentTime().toString() + " " +
            QDate::currentDate().toString(Qt::DefaultLocaleShortDate),
            Qt::black);

    return true;
}

//читаем грамматику из файла, переписываем ее в память и составляем словарь для грамматики
bool PrecedenceTableBuilder::readGrammar()
{
    QString qstr;
    string str;//строка для считывания нового симвога грамматики
    QFile f(GRAMMAR_FILE);
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!f.isOpen()) {
        //cerr << "Unable to open file with grammar ;(" << endl;
        emit infoMsg("Не удалось прочитать грамматику", Qt::darkRed);
        return false;
    }
    QTextStream file(&f);

    //читаем грамматику из файла
    file >> qstr;
    str = qstr.toStdString();
    while(true) {
        file >> qstr;
        str = qstr.toStdString();
        if(file.atEnd())
            break;
        //проверяем, не был ли считат этот элемент ранее
        bool contains = false;
        for (unsigned int i = 0; i < grammar_symbols.size(); i++) {
            if (grammar_symbols.at(i) == str) {
                contains = true;
                break;
            }
        }
        //если нет, то запысываем его в массив символов грамматики
        if (!contains && str != GRAMMAR_ASSIGMENT && str != RULE_STOP_SYMBOL)
            grammar_symbols.push_back(str);
    }
    grammar_symbols.push_back("#");

    //переписываем построчно грамматику в массив строк
//    f.close();//закрываем файл
//    f.open(QIODevice::ReadOnly | QIODevice::Text);
//    if (!f.isOpen()) {
//        emit infoMsg("Не удалось прочитать грамматику", Qt::darkRed);
//        return false;
//    }

//    QTextStream file1(&f);
    file.seek(0);

    vector<string> g_line;
    while (true) {
        file >> qstr;
        str = qstr.toStdString();
        if(file.atEnd()) {
            if (g_line.size() != 0) {
                if (g_line.size() <= 2) {
                    emit infoMsg("Ошибка в грамматике", Qt::darkRed);
                    return -1;
                }
                grammar.push_back(g_line);
            }
            break;
        }
        if (str == RULE_STOP_SYMBOL) {
            if (g_line.size() <= 2) {
                emit infoMsg("Ошибка в грамматике", Qt::darkRed);
                return -1;
            }
            grammar.push_back(g_line);
            g_line.clear();
            continue;
        }
        g_line.push_back(str);
    }

    f.close();//закрываем файл

    return true;
}

//заполняем таблицу предшествований
bool PrecedenceTableBuilder::fillTable()
{
    //ставим знак "="
    //проход по строкам грамматики
    for (unsigned int i = 0; i  < grammar.size(); i++) {
        //проход по символам грамматики в каждой строке
        for (unsigned int j = 2; j < grammar[i].size()-1; j++) {
            if (!putToTable(grammar[i][j], grammar[i][j+1], T_EQUAL)) {
                emit infoMsg("Построение таблици закончено на строке" + QString::number(i+1), Qt::darkRed);
                return false;
            }
        }
    }

    //ставим знак ">"
    //находим равенства, у которых 1й символ - нетерминал
    for (unsigned int i = 0; i < grammar_symbols.size(); i++) {
        for (unsigned int j = 0; j < grammar_symbols.size(); j++) {
            if (g_table[i][j] == T_EQUAL && !isTeriminal(grammar_symbols[i])) {
                vector<string> first;
                vector<string> last;
                getFirst(grammar_symbols[j], &first);
                getLast(grammar_symbols[i], &last);
                //если 2й нетерминал
                if (!isTeriminal(grammar_symbols[j])) {
                    for (unsigned int k = 0; k < last.size(); k++) {
                        for (unsigned int t = 0; t < first.size(); t++) {
                            if (!putToTable(last[k], first[t], T_MORE)) {
                                emit infoMsg("Построение таблици закончено с ошибками", Qt::darkRed);
                                return false;
                            }
                        }
                    }
                }
                else {
                    for (unsigned int k = 0; k < last.size(); k++) {
                        if (!putToTable(last[k], grammar_symbols[j], T_MORE)) {
                            emit infoMsg("Построение таблици закончено с ошибками", Qt::darkRed);
                            return false;
                        }
                    }
                }
            }
        }
    }

    //ставим знак "<"
    //находим равенства, у которых 2й символ - нетерминал
    for (unsigned int i = 0; i < grammar_symbols.size(); i++) {
        for (unsigned int j = 0; j < grammar_symbols.size(); j++) {
            if (g_table[i][j] == T_EQUAL && !isTeriminal(grammar_symbols[j])) {
                vector<string> first;
                vector<string> last;
                getFirst(grammar_symbols[j], &first);
                getLast(grammar_symbols[i], &last);

                for (unsigned int k = 0; k < first.size(); k++) {
                    if (!putToTable(grammar_symbols[i], first[k], T_LESS)) {
                        emit infoMsg("Построение таблици закончено с ошибками", Qt::darkRed);
                        return false;
                    }
                }
            }
        }
    }

    //ставим отношеня для '#'
    for (unsigned int j = 0; j < grammar_symbols.size()-1; j++) {
        if (!putToTable("#", grammar_symbols[j], T_LESS)) {
            emit infoMsg("Построение таблици закончено с ошибками", Qt::darkRed);
            return false;
        }
    }
    for (unsigned int i = 0; i < grammar_symbols.size()-1; i++) {
        if (!putToTable(grammar_symbols[i], "#", T_MORE)) {
            emit infoMsg("Построение таблици закончено с ошибками", Qt::darkRed);
            return false;
        }
    }

    return true;
}

//получить FIRST+
void PrecedenceTableBuilder::getFirst(string symbol, vector<string> *first_symbols)
{
    if (isTeriminal(symbol))//если символ терминальный, то вываливаемся, для него нельзя найти FIRST+
        return;

    for (unsigned int i = 0; i < grammar.size(); i++) {
        //исщем правило, начинающееся с нужного нам символа
        if (grammar[i][0] == symbol) {
            //если нашли, то добавляем к списку first_symbols 3й символ правила и продолжаем поиск
            //только если его еще нет в списке
            bool contains = false;
            for (unsigned int j = 0; j < first_symbols->size(); j++) {
                if (first_symbols->at(j) == grammar[i][2]) {
                    contains = true;
                    break;
                }
            }
            if (!contains) {
                first_symbols->push_back(grammar[i][2]);
                //если он тоже терминальный, то ищем и для него FIRST+
                if (!isTeriminal(grammar[i][2])) {
                    getFirst(grammar[i][2], first_symbols);
                }
            }
        }
    }
}

//получить LAST+
void PrecedenceTableBuilder::getLast(string symbol, vector<string> *last_symbols)
{
    if (isTeriminal(symbol))//если символ терминальный, то вываливаемся, для него нельзя найти FIRST+
        return;

    for (unsigned int i = 0; i < grammar.size(); i++) {
        //исщем правило, начинающееся с нужного нам символа
        if (grammar[i][0] == symbol) {
            //если нашли, то добавляем к списку last_symbols последний символ правила и продолжаем поиск
            //только если его еще нет в списке
            bool contains = false;
            for (unsigned int j = 0; j < last_symbols->size(); j++) {
                if (last_symbols->at(j) == grammar[i][grammar[i].size()-1]) {
                    contains = true;
                    break;
                }
            }
            if (!contains) {
                last_symbols->push_back(grammar[i][grammar[i].size()-1]);
                //если он тоже терминальный, то ищем и для него FIRST+
                if (!isTeriminal(grammar[i][grammar[i].size()-1])) {
                    getLast(grammar[i][grammar[i].size()-1], last_symbols);
                }
            }
        }
    }
}

//терминальный ли символ
bool PrecedenceTableBuilder::isTeriminal(string symbol)
{
    //нетерминальные символы записываются в < > и содержат не менее 3х символов
    //т.к. 2 символа кавычек и хотя бы 1 символ непосредственно на символ грамматики

    if (symbol.size() <= 2)
        return true;
    else if (symbol[0] == '<' && symbol[symbol.size()-1] == '>')
        return false;
    return true;
}

//ставим нужный символ в таблицу
bool PrecedenceTableBuilder::putToTable(string symbol1, string symbol2, int relations)
{
    int p1 = getGrammarSymbolNumber(symbol1);
    int p2 = getGrammarSymbolNumber(symbol2);
    if (p1 == -1 || p2 == -1) {
        emit infoMsg("Неверный символ в грамматике", Qt::darkRed);
        return false;
    }
    if (g_table[p1][p2] != T_DEFAULT) {
        if (g_table[p1][p2] == relations) {
            //cerr << "Table already containes such relation. Nothin changed. Skiped." << endl;
            return true;
        }
        else {
            emit infoMsg(QString("Таблица уже содержит значение в текущей ячейке ( ") +
                 QString(symbol1.c_str()) + " " + getRalationsSymbol(g_table[p1][p2])  + " " +
                         QString(symbol2.c_str()) + " ). " +
                 "Trying to set ( " +
                 QString(symbol1.c_str()) + " " + getRalationsSymbol(relations)  + " " +
                         QString(symbol2.c_str()) + " ) failed.",
                 Qt::darkRed);
            return false;
        }
    }
    else {
        g_table[p1][p2] = relations;
    }
    return true;
}

//получаем номер символа грамматики
int PrecedenceTableBuilder::getGrammarSymbolNumber(string symbol)
{
    unsigned int number = -1;
    for (unsigned int i = 0; i < grammar_symbols.size(); i++) {
        if (grammar_symbols[i] == symbol) {
            number = i;
            break;
        }
    }
    return number;
}

//возвращаем символ отношения по коду
char PrecedenceTableBuilder::getRalationsSymbol(int relations)
{
    switch (relations)
    {
    case T_DEFAULT:
        return ' ';
        break;
    case T_EQUAL:
        return '=';
        break;
    case T_MORE:
        return '>';
        break;
    case T_LESS:
        return '<';
        break;
    }
    return ' ';
}

int** PrecedenceTableBuilder::getPrecTable()
{
    return g_table;
}

int PrecedenceTableBuilder::getGrammarSize()
{
    return grammar_symbols.size();
}

vector<string> PrecedenceTableBuilder::getGrammarSymbols()
{
    return grammar_symbols;
}
