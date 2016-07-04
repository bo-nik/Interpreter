#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

#include <QString>
#include <QStringList>
#include <QList>
#include <QTime>
#include <QColor>

#define ID_NUMBER       34
#define CONST_NUMBER    35
#define LABEL_NUMBER    36

struct Lexeme
{
    Lexeme(QString _lex, int _l, int _n, int _id) : lex(_lex), l(_l), n(_n), id(_id) {}
    QString lex;
    int l;
    int n;
    int id;
};

class LexicalAnalyzer : public QObject
{
    Q_OBJECT
public:
    explicit LexicalAnalyzer(QObject *parent = 0);

    void setData(QString data);
    int isSuccessful() {return !errors;}
    void getData(QList<Lexeme> *_oTable, QStringList *_vTable, QStringList *_cTable, QStringList *_lTable);

    void analyze();
    
signals:
    void infoMsg(QString msg, QColor color);
    void errorMsg(int line, QString msg, QColor color);
    void warningMsg(int line, QString msg, QColor color);

private:
    void prepare();//очистка всех списков и т.д.
    int charClass(char ch);//возвращает класс символа
    void proceedBuffer(QString buf, int line, int n, int ch);//анализирует содержимое буфера
    void checkIdentificators();//проверка объявлений переменных
    void checkLabels();//проверка меток

private:
    QString data;

    QList<Lexeme> oTable;
    QStringList vTable;
    QStringList cTable;
    QStringList lTable;

    int errors;
    int warnings;

    //для поиска меток, по которым нет переходов, и необъявленных меток
    QStringList m_labels_defined;//объявленные метки
    QList<int> m_labels_defined_lines;//строки, в которых находятся метки
    QStringList m_labels_called;//метки, по которым пытались перейти
    QList<int> m_labels_called_lines;//строки, в которых находятся метки
    bool beginFounded;//началась ли секция begin
    
};

#endif // LEXICALANALYZER_H
