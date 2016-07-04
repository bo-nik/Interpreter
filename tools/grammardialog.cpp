#include "grammardialog.h"

GrammarDialog::GrammarDialog(QWidget *parent) :
    QDialog(parent)
{
    grammarTextEdit = new QTextEdit;
    grammarTextEdit->setLineWrapMode(QTextEdit::NoWrap);

    grammarTextEdit->setReadOnly(true);

    okButton = new QPushButton("Ок");

    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(okButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(grammarTextEdit);
    mainLayout->addLayout(btnLayout);

    setLayout(mainLayout);

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

    QFile file(":/data/grammar.info");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!file.isOpen()) {
        QMessageBox::warning(this, "Ошибочка", "Не удалось прочитать грамматику", QMessageBox::Ok);
        return;
    }
    QTextStream fs(&file);
    grammarTextEdit->setText(fs.readAll());
    file.close();

    setMinimumSize(400, 500);

    setWindowTitle("Грамматика");
}
