#ifndef POLISBUILDDIALOG_H
#define POLISBUILDDIALOG_H

#include <QtWidgets>
#include <QList>
#include <QStringList>

class PolisBuildDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PolisBuildDialog(QWidget *parent = 0);
    void setData(QList<QStringList> data);

private:
    QTreeWidget *tableWidget;
    QPushButton *okButton;
    
};

#endif // POLISBUILDDIALOG_H
