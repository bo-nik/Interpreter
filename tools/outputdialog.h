#ifndef OUTPUTDIALOG_H
#define OUTPUTDIALOG_H

#include <QtWidgets>
#include <QList>
#include <QStringList>

class OutputDialog : public QDialog
{
    Q_OBJECT
public:
    explicit OutputDialog(QWidget *parent = 0);
    void setData(QList<QStringList> data);

private:
    QTreeWidget *tableWidget;
    QPushButton *okButton;
    
};

#endif // OUTPUTDIALOG_H
