#ifndef GRAMMARDIALOG_H
#define GRAMMARDIALOG_H

#include <QtWidgets>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

class GrammarDialog : public QDialog
{
    Q_OBJECT
public:
    explicit GrammarDialog(QWidget *parent = 0);
    
private:
    QTextEdit *grammarTextEdit;
    QPushButton *okButton;
    
};

#endif // GRAMMARDIALOG_H
