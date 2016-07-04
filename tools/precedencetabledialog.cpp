#include "precedencetabledialog.h"

PrecedenceTableDialog::PrecedenceTableDialog(QWidget *parent) :
    QDialog(parent)
{
    tableWidget = new QTableWidget;
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    okButton = new QPushButton("Ок");

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(okButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tableWidget);
    mainLayout->addLayout(btnLayout);

    setLayout(mainLayout);

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    setMinimumSize(500, 500);

    setWindowTitle("Таблица предшествований");
}

void PrecedenceTableDialog::setSize(int sz)
{
    tableWidget->setRowCount(sz);
    tableWidget->setColumnCount(sz);
    size = sz;
}

void PrecedenceTableDialog::setGrammar(vector<string> g)
{
    QStringList labels;
    for (int i = 0; i < size; i++) {
        labels << g[i].c_str();
    }
    tableWidget->setHorizontalHeaderLabels(labels);
    tableWidget->setVerticalHeaderLabels(labels);
}

void PrecedenceTableDialog::setTable(int **t)
{
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            QString s;
            if (t[i][j] == 1)
                s = "=";
            else if (t[i][j] == 2)
                s = ">";
            else if (t[i][j] == 3)
                s = "<";
            else
                s = "";

            QTableWidgetItem *item = new QTableWidgetItem(s);
            tableWidget->setItem(i, j, item);
        }
    }
}
