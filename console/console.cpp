#include "console.h"

Console::Console(QWidget *parent) :
    QPlainTextEdit(parent)
{
    prompt = "program> ";

    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::white);
    setPalette(p);

    //insertPrompt(false);
    isLocked = true;
}

void Console::keyPressEvent(QKeyEvent *event)
{
    if (isLocked)
        return;

    if (((event->key() >= '0' && event->key() <= '9') || event->key() == '.' || event->key() == '-')
            && (event->modifiers() == Qt::NoModifier || event->modifiers() == Qt::ShiftModifier))
        QPlainTextEdit::keyPressEvent(event);

    if (event->key() == Qt::Key_Backspace
            && event->modifiers() == Qt::NoModifier
            && textCursor().positionInBlock() > prompt.length())
        QPlainTextEdit::keyPressEvent(event);

    if (event->key() == Qt::Key_Return && event->modifiers() == Qt::NoModifier)
        onEnter();

    QString cmd = textCursor().block().text().mid(prompt.length());
    emit onChange(cmd);
}

void Console::mousePressEvent(QMouseEvent *)
{
    setFocus();
}

void Console::mouseDoubleClickEvent(QMouseEvent *) {}

void Console::contextMenuEvent(QContextMenuEvent *) {}

void Console::onEnter()
{
    if (textCursor().positionInBlock() == prompt.length()) {
        //insertPrompt();
        return;
    }
    QString cmd = textCursor().block().text().mid(prompt.length());
    isLocked = true;
    //textCursor().insertBlock();
    scrollDown();
    emit onCommand(cmd);
}

void Console::output(QString s, bool insertNewBlock)
{
    if (insertNewBlock)
        textCursor().insertBlock();
    QTextCharFormat format;
    format.setForeground(Qt::white);
    textCursor().setBlockCharFormat(format);
    textCursor().insertText(s);
    scrollDown();
    setFocus();
}

void Console::insertPrompt(bool insertNewBlock)
{
    if (insertNewBlock) {
        textCursor().insertBlock();
    }
    QTextCharFormat format;
    format.setForeground(Qt::green);
    textCursor().setBlockCharFormat(format);
    textCursor().insertText(prompt);
    scrollDown();
    setFocus();
}

void Console::scrollDown()
{
    QScrollBar *vbar = verticalScrollBar();
    vbar->setValue(vbar->maximum());
}

void Console::getValue(QString promt)
{
    this->prompt = promt;
    if (promt != NULL)
        insertPrompt();
    isLocked = false;
}

void Console::startExec()
{
    clear();
    output("Выполнение начато в " +
           QTime::currentTime().toString() + " " +
           QDate::currentDate().toString(Qt::DefaultLocaleShortDate),
           false);
}

void Console::finishExec()
{
    output("Выполнение завершено в " +
           QTime::currentTime().toString() + " " +
           QDate::currentDate().toString(Qt::DefaultLocaleShortDate));
}

void Console::finishExecWithErrors()
{
    output("Выполнение аварийно завершено в " +
           QTime::currentTime().toString() + " " +
           QDate::currentDate().toString(Qt::DefaultLocaleShortDate));
}
