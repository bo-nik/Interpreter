#ifndef CONSOLE_H
#define CONSOLE_H

#include <QtWidgets>
#include <QTime>

class Console : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit Console(QWidget *parent = 0);
    void output(QString s, bool insertNewBlock = true);
    void scrollDown();
    void getValue(QString promt = NULL);
    void startExec();
    void finishExec();
    void finishExecWithErrors();

protected:
    void keyPressEvent(QKeyEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseDoubleClickEvent(QMouseEvent *);
    void contextMenuEvent(QContextMenuEvent *);

private:
    QString prompt;
    bool isLocked;

    void onEnter();
    void insertPrompt(bool insertNewBlock = true);

signals:
    void onCommand(QString);
    void onChange(QString);
};

#endif // CONSOLE_H
