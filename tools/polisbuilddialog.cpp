#include "polisbuilddialog.h"

PolisBuildDialog::PolisBuildDialog(QWidget *parent) :
    QDialog(parent)
{
    tableWidget = new QTreeWidget;
    tableWidget->setColumnCount(3);
    tableWidget->setHeaderLabels(QStringList() << "Вход" << "Стек" << "Выход");
    tableWidget->setAlternatingRowColors(true);
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

    setWindowTitle("Журнал синтаксического анализа");

    setMinimumSize(400, 500);
}

void PolisBuildDialog::setData(QList<QStringList> data)
{
    for (int i = 0; i < data.size(); i++) {
        QTreeWidgetItem *item = new QTreeWidgetItem(tableWidget);
        item->setText(0, data[i][0]);
        item->setText(1, data[i][1]);
        item->setText(2, data[i][2]);
        tableWidget->addTopLevelItem(item);
    }
}
