#ifndef PRIORITYSHOWDIALOG_H
#define PRIORITYSHOWDIALOG_H

#include <QtWidgets>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>

class PriorityShowDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PriorityShowDialog(QWidget *parent = 0);
    
private:
    QTreeWidget *tableWidget;
    QPushButton *okButton;
    
};

#endif // PRIORITYSHOWDIALOG_H
