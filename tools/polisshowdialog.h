#ifndef POLISSHOWDIALOG_H
#define POLISSHOWDIALOG_H

#include <QtWidgets>
#include <QList>
#include <string>

using namespace std;

class PolisShowDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PolisShowDialog(QWidget *parent = 0);
    void setData(QList<string> data);
    
private:
    QTextEdit *textEdit;
    QPushButton *okButton;
    
};

#endif // POLISSHOWDIALOG_H
