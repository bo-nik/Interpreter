#include "lexicalanalyzer.h"

#define STATES_COUNT        9  //кол-во состояний автомата
#define CHAR_CLASSES_COUNT  9  //кол-во классов символов
#define LEXEMS_COUNT        34 //кол-во лексем

int M[STATES_COUNT][CHAR_CLASSES_COUNT] = {{   1,    2,  101,    8,    7,    5,    6,    3,    0},
                                           {   1,    1,  100,  100,  100,  100,  100,  100,  100},
                                           {-350,    2, -350, -350, -350, -350, -350,    4, -350},
                                           { 666,    4,  666,  666,  666,  666,  666,  666,  666},
                                           {-350,    4, -350, -350, -350, -350, -350, -350, -350},
                                           {-160, -160, -160,  -18, -160, -160, -160, -160, -160},
                                           {-170, -170, -170,  -19, -170, -170, -170, -170, -170},
                                           { 666,  666,  666,  -21,  666,  666,  666,  666,  666},
                                           {-260, -260, -260,  -20, -260, -260, -260, -260, -260}};

const char *lexemes[] = {"program", "var", "begin", "end", "float", "if", "then", "goto", "do", "to", "next", "read",
                         "write", "or", "and", "not", ">", "<", ">=", "<=", "==", "!=", "+", "-", "*", "/", "=", "(",
                         ")", "[", "]", ",", ":", "\n"};

LexicalAnalyzer::LexicalAnalyzer(QObject *parent) : /*QThread*/QObject(parent)
{
    errors = 0;
    warnings = 0;
}

void LexicalAnalyzer::setData(QString data)
{
    this->data = data;
}

void LexicalAnalyzer::analyze()
{
    emit infoMsg(QString("Лексический разбор начат в ") +
                 QTime::currentTime().toString() + " " +
                 QDate::currentDate().toString(Qt::DefaultLocaleShortDate),
                 Qt::black);

    prepare();//очистка всех списков

    QString buffer;//буфер для текущей лексемы
    int line = 0;//номер текущей строки
    int state = 0;//текущее состояние автомата
    char ch;//переменная для текущего символа
    bool read = true;//считывать ли следущий символ
    int chClass = 0;//переменная для класса текущего символа
    int s = 0;//сюда из матрицы переходов звписываем следущее состояние

    for(int i = 0; i < data.size()+1; i++)
    {
        if(read)
            ch = data[i].toLatin1();
        else
            i--;
        if(ch == '\r')
            continue;
        if(i == data.size())
            ch = ' ';
        chClass = charClass(ch);//получаем класс символа
        if(chClass == STATES_COUNT-1 && ch != ' ' && ch != 9 /*tab*/ && read) { //если пробел, ошибку выдавать не нужно
            emit errorMsg(line + 1, QString("Недопустимый символ < %1 >").arg(ch), Qt::black);
            errors++;
        }
        read = true;

        //обработка значения в матрице переходов
        s = M[state][chClass];//из матрицы переходов получем следущее состояние авомата
        if(s >= 0 && s < STATES_COUNT)//если в матрице номер сосотояния, то символ записываем в буфер и переходим в новое состояние
        {
            if(s == 0)//если нужно перейти в 0, значит встретили недопустимый символ и ошибка уже выдана, анализируем буфер и переходим в начало
            {
                proceedBuffer(buffer, line, -1, ch);
                buffer.clear();
            }
            else//все норм., добавляем символ в буфер и переходим в новое состояние
                buffer.push_back(ch);
            state = s;
            continue;
        }
        else if(s == 100)//в буфере id, label или j (какая-то лексема)
        {
            proceedBuffer(buffer, line, s, ch);
            buffer.clear();
            read = false;
            state = 0;
        }
        else if(s == 101)//в буфере одиночный разделитель
        {
            buffer.push_back(ch);
            proceedBuffer(buffer, line, s, ch);
            buffer.clear();
            state = 0;
            if(ch == '\n')
                line++;//если пришел символ новой строки, увеличиваем счетчик строк
        }
        else if(s == 102)//в буфере + или -. Нужно опеределить, он относится к костанте или это знак операции
        {
            //if(oTable.size() > 0 && (oTable[oTable.size()-1].n == 34 || oTable[oTable.size()-1].n == 35))//перед знаком id или const, значит это знак операци
            if(oTable.size() > 0 && oTable[oTable.size()-1].n != 26)//перед знаком =, т.е. только при присвоении может быть отрицательная константа
            {
                int n = ch=='+'?22:23;
                proceedBuffer(buffer, line, n, ch);
                buffer.clear();
                state = 0;
                read = false;
            }
            else
            {
                buffer.push_back(ch);
                state = 4;
            }
        }
        else if(s < 0)//в матрице номер лексемы (|s|), находящейся в буфере
        {
            s *= -1;
            if(s < LEXEMS_COUNT)//значит следущий символ нужно считвать
            {
                buffer.push_back(ch);
                proceedBuffer(buffer, line, s, ch);
            }
            else//значит следущий символ не нужно нужно считвать, номер лексемы = s/10
            {
                s /= 10;
                proceedBuffer(buffer, line, s, ch);
                read = false;
            }
            buffer.clear();
            state = 0;
        }
        else if(s == 666)//ошибка, неожиданный после '.' или '!' символ
        {
            if(ch == '\n') {
                emit errorMsg(line + 1, QString("Неожиданный символ < %1 > после < %2 >").arg('~').arg(buffer[buffer.size()-1]),
                        Qt::black);
                errors++;
            }
            else {
                emit errorMsg(line + 1, QString("Неожиданный символ < %1 > после < %2 >").arg(ch).arg(buffer[buffer.size()-1]),
                        Qt::black);
                errors++;
            }
            buffer.clear();
            read = false;
            state = 0;
        }
    }

    checkIdentificators();
    checkLabels();

    if(!errors && !warnings)
        emit infoMsg(QString("Лексический разбор окончен УСПЕШНО в ") +
                     QTime::currentTime().toString() + " " +
                     QDate::currentDate().toString(Qt::DefaultLocaleShortDate),
                     Qt::black);
    else if(errors)
        emit infoMsg(QString("Лексический разбор окончен в ") +
                     QTime::currentTime().toString() + " " +
                     QDate::currentDate().toString(Qt::DefaultLocaleShortDate) +
                     " с ОШИБКАМИ\nОшибок: " + QString::number(errors) +
                     ", Предупреждений: " + QString::number(warnings),
                     Qt::darkRed);
    else if(!errors && warnings)
        emit infoMsg(QString("Лексический разбор окончен в "  +
                     QTime::currentTime().toString() + " " +
                     QDate::currentDate().toString(Qt::DefaultLocaleShortDate)) +
                     " с ПРЕДУПРЕЖДЕНИЯМИ\nПредупреждений: " + QString::number(warnings),
                     Qt::darkBlue);
}

void LexicalAnalyzer::prepare()
{
    oTable.clear();
    vTable.clear();
    cTable.clear();
    lTable.clear();

    errors = 0;
    warnings = 0;
    beginFounded = false;
    m_labels_defined.clear();
    m_labels_defined_lines.clear();
    m_labels_called.clear();
    m_labels_called_lines.clear();
}

int LexicalAnalyzer::charClass(char ch)
{
    if((ch >= 97 /*a*/ && ch <= 122 /*z*/) || (ch >= 65 /*A*/ && ch <= 90 /*Z*/) || ch == '_')
        return 0;
    else if(ch >= 48 /*0*/ && ch <= 57 /*9*/)
        return 1;
    else if(ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == ',' || ch == ':' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '\n')
        return 2;
    else if(ch == '=')
        return 3;
    else if(ch == '!')
        return 4;
    else if(ch == '>')
        return 5;
    else if(ch == '<')
        return 6;
    else if(ch == '.')
        return 7;
    else//все остальные символы
        return 8;
}

void LexicalAnalyzer::proceedBuffer(QString buf, int line, int n, int ch)
{
    int sz = buf.size();
    if(sz == 0)
        return;

    //переписываем лексему из QList в char
    char lex[sz+1];
    lex[sz] = '\0';
    for(int i = 0; i < sz; i++)
        lex[i] = buf[i].toLatin1();

    if(n == 100 || n == 101)
    {
        for(n = 0; n < LEXEMS_COUNT; n++)
            if(strcmp(lex, lexemes[n]) == 0)
                break;
    }
    if(n == 2)//begin
        beginFounded = true;

    int id = -1;
    if(n == 35)//константа, нужно занести в таблицу констант
    {
        if(!cTable.contains(lex))
        {
            cTable.push_back(lex);
            id = cTable.indexOf(lex);
            //emit newConstant(lex, id);
        }
        id = cTable.indexOf(lex);
    }
    if(n == LEXEMS_COUNT)//id или метка, нужно узнать и занести в соответсвующую таблицу
    {
        //проверяем, это метка или идентификатор
        bool last_goto = false;
        if(oTable.size() != 0 && oTable.back().lex.compare("goto\0") == 0)
            last_goto = true;

        if(ch == ':' && last_goto)//это ошибка в синтаксисе метки (goto label:)
        {
            emit errorMsg(line + 1, QString("Ошибка в синтаксисе метки: goto %2:").arg(lex), Qt::black);
            errors++;
            return;
        }
        if((ch == ':' || last_goto) && beginFounded)//это метка
        {
            n = 36;
            if(!lTable.contains(lex))
            {
                lTable.push_back(lex);
                id = lTable.indexOf(lex);
                //emit newLabel(lex, id);
            }
            id = lTable.indexOf(lex);

            //записываем метку либо в объявленные, либо в вызванные
            if(ch == ':')//это объявление
            {
                m_labels_defined.push_back(lex);
                m_labels_defined_lines.push_back(line);
            }
            else
            {
                m_labels_called.push_back(lex);
                m_labels_called_lines.push_back(line);
            }
        }
        else//это идентификатор
        {
            n = 34;
            if(!vTable.contains(lex))
            {
                vTable.push_back(lex);
                id = vTable.indexOf(lex);
                //emit newIdentificator(lex, id);
            }
            id = vTable.indexOf(lex);
        }
    }

    //emit newLexeme(lex, line+1, n, id);
    oTable.push_back(Lexeme(lex, line, n, id));//сохраняем все лексемы для потомков
}

void LexicalAnalyzer::checkIdentificators()
{
    QList<Lexeme> definedIdentificators;
    bool after_var = false;
    int after_float_lex_n = 0;
    for(int i = oTable.size()-1; i >= 0; i--)
    {
        if(oTable[i].n == 4)//слово float
        {
            after_float_lex_n = i;
            break;
        }
    }

    for(int i = 0; i < oTable.size(); i++)
    {
        if(oTable[i].n == 1)//после слова var
            after_var = true;
        if(i == after_float_lex_n)
            break;
        if(after_var && oTable[i].n == 34)
            definedIdentificators.push_back(oTable[i]);
    }

    //проверка необъявленных переменных
    int start = 0;
    for(int i = 0; i < oTable.size(); i++)
    {
        if(oTable[i].n == 0)
        {
            start = i+2;
            break;
        }
    }
    for(int i = start; i < oTable.size(); i++)
    {
        if(oTable[i].n != 34)
            continue;
        int j = 0;
        for(j = 0; j < definedIdentificators.size(); j++)
        {
            if(oTable[i].lex == definedIdentificators[j].lex)
                break;
        }
        if(j == definedIdentificators.size()) {
            emit errorMsg(oTable[i].l+1, QString("Необъявленная переменная < %1 >").arg(oTable[i].lex), Qt::black);
            errors++;
        }
    }

    //проверка множественного объявления
    QList<Lexeme> checked;
    for(int i = 0; i < definedIdentificators.size(); i++)
    {
        int k = 0;
        for(k = 0; k < checked.size(); k++)
            if(definedIdentificators[i].lex.compare(checked[k].lex) == 0)
                break;
        if(k == checked.size())
        {
            for(int j = i+1; j < definedIdentificators.size(); j++)
            {
                if(definedIdentificators[i].lex.compare(definedIdentificators[j].lex) == 0) {
                    emit errorMsg(definedIdentificators[j].l+1,
                                  QString("Перемнная < %1 > уже объявлена").arg(definedIdentificators[j].lex),
                                  Qt::black);
                    errors++;
                }
            }
            checked.push_back(definedIdentificators[i]);
        }
    }

    //проверка неиспольземых переменных
    QList<Lexeme> singleDefined;
    for(int i = 0; i < definedIdentificators.size(); i++)
    {
        int j = 0;
        for(j = 0; j < singleDefined.size(); j++)
            if(singleDefined[j].lex.compare(definedIdentificators[i].lex) == 0)
                break;
        if(j == singleDefined.size())
            singleDefined.push_back(definedIdentificators[i]);
    }
    for(int i = 0; i < singleDefined.size(); i++)
    {
        int j = 0;
        for(j = after_float_lex_n; j < oTable.size(); j++)
        {
            if(oTable[j].n != 34)
                continue;
            if(singleDefined[i].lex.compare(oTable[j].lex) == 0)
                break;
        }
        if(j == oTable.size()) {
            emit warningMsg(singleDefined[i].l+1, QString("Неиспользованная переменная %1").arg(singleDefined[i].lex),
                            Qt::black);
            warnings++;
        }
    }
}

void LexicalAnalyzer::checkLabels()
{
    //проверка множественного объявления меток
    QStringList checked;
    for(int i = 0; i < m_labels_defined.size(); i++)
    {
        int k = 0;
        for(k = 0; k < checked.size(); k++)
            if(m_labels_defined[i].compare(checked[k]) == 0)
                break;
        if(k == checked.size())
        {
            for(int j = i+1; j < m_labels_defined.size(); j++)
            {
                if(m_labels_defined[i].compare(m_labels_defined[j]) == 0)
                {
                    emit errorMsg(m_labels_defined_lines[j]+1, QString("Метка < %1 > уже объявлена").arg(m_labels_defined[j]),
                                  Qt::black);
                    errors++;
                }
            }
            checked.push_back(m_labels_defined[i]);
        }
    }

    for(int i = 0; i < m_labels_called.size(); i++)
        if(!m_labels_defined.contains(m_labels_called[i])) {
            emit errorMsg(m_labels_called_lines[i]+1, QString("Переход по несуществующей метке %1").arg(m_labels_called[i]),
                          Qt::black);
            errors++;
        }
    for(int i = 0; i < m_labels_defined.size(); i++)
        if(!m_labels_called.contains(m_labels_defined[i])) {
            emit warningMsg(m_labels_defined_lines[i]+1, QString("Неиспользованная объявленная метка %1").arg(m_labels_defined[i]),
                            Qt::black);
            warnings++;
        }

}

void LexicalAnalyzer::getData(QList<Lexeme> *_oTable, QStringList *_vTable, QStringList *_cTable, QStringList *_lTable)
{
    *_oTable = oTable;
    *_vTable = vTable;
    *_cTable = cTable;
    *_lTable = lTable;
}
