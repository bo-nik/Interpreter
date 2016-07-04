#include "priorityshowdialog.h"

PriorityShowDialog::PriorityShowDialog(QWidget *parent) :
    QDialog(parent)
{
    tableWidget = new QTreeWidget;
    tableWidget->setColumnCount(2);
    tableWidget->setHeaderLabels(QStringList() << tr("Символ") << tr("Приоритет"));
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

    QFile file(":/data/priority.table");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!file.isOpen()) {
        QMessageBox::warning(this, "Ошибочка", "Не удалось прочитать таблицу приоритетов", QMessageBox::Ok);
        return;
    }
    QTextStream fs(&file);

    while (true) {
        if (fs.atEnd()) {
            break;
        }

        QString s1;
        QString s2;
        fs >> s1;
        fs >> s2;
        if (s1 == "") {
            continue;
        }
        QTreeWidgetItem *i = new QTreeWidgetItem(tableWidget);
        i->setText(0, s1);
        i->setText(1, s2);
        tableWidget->addTopLevelItem(i);
    }

    file.close();

    setWindowTitle("Таблица приоритетов");

    setMinimumSize(300, 400);
}
