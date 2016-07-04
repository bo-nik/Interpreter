#include "polisshowdialog.h"

PolisShowDialog::PolisShowDialog(QWidget *parent) :
    QDialog(parent)
{
    textEdit = new QTextEdit;
    //textEdit->setLineWrapMode(QTextEdit::NoWrap);

    textEdit->setReadOnly(true);

    okButton = new QPushButton("Ок");

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(okButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(textEdit);
    mainLayout->addLayout(btnLayout);

    setLayout(mainLayout);

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    setWindowTitle("ПОЛИЗ");
}

void PolisShowDialog::setData(QList<string> data)
{
    QString s;
    for (int i = 0; i < data.size(); i++) {
        s += data[i].c_str() + QString(" ");
    }
    textEdit->setText(s);
}
