#ifndef PRECEDENCETABLEDIALOG_H
#define PRECEDENCETABLEDIALOG_H

#include <QtWidgets>
#include <vector>
#include <string>

using namespace std;

class PrecedenceTableDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PrecedenceTableDialog(QWidget *parent = 0);

    void setSize(int sz);
    void setGrammar(vector<string> g);
    void setTable(int **t);
    
private:
    QTableWidget *tableWidget;
    QPushButton *okButton;

    int size;
    
};

#endif // PRECEDENCETABLEDIALOG_H
