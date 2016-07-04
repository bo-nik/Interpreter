#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    analyzed = false;
    analyzeAndRun = false;

    setupUi();
    createActions();
    createMenus();
    //createToolBars();

    analyzer = new Analyzer;
    evaluator = new Evaluator;

    grammarDialog = new GrammarDialog;
    priorityShowDialog = new PriorityShowDialog;
    precedenceTableDialog = new PrecedenceTableDialog;
    outputDialog = new OutputDialog;
    polisBuildDialog = new PolisBuildDialog;
    polisShowDialog = new PolisShowDialog;

    connect(analyzer, SIGNAL(infoMsg(QString,QColor)), this, SLOT(infoMsg(QString,QColor)));
    connect(analyzer, SIGNAL(errorMsg(int,QString,QColor)), this, SLOT(errorMsg(int,QString,QColor)));
    connect(analyzer, SIGNAL(warningMsg(int,QString,QColor)), this, SLOT(warningMsg(int,QString,QColor)));
    connect(analyzer, SIGNAL(finished()), this, SLOT(analyzeFinished()));

    connect(console, SIGNAL(onCommand(QString)), this, SLOT(cmdFromConsole(QString)));
    connect(this, SIGNAL(idForRead(double)), evaluator, SLOT(idForRead(double)));

    connect(evaluator, SIGNAL(programMsg(QString)), this, SLOT(programMsg(QString)));
    connect(evaluator, SIGNAL(finished()), this, SLOT(evaluateFinished()));
    connect(evaluator, SIGNAL(needVarValForRead(QString)), this, SLOT(needVarValueToRead(QString)));

    connect(errorsListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(errorItemClicked(QListWidgetItem*)));
    connect(errorsListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(errorItemClicked(QListWidgetItem*)));
    connect(warningsListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(warningItemClicked(QListWidgetItem*)));
    connect(warningsListWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)),
            this, SLOT(warningItemClicked(QListWidgetItem*)));

    newDoc();
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::createActions()
{
    newAction = new QAction("&Создать", this);
    newAction->setIcon(QIcon(":/icons/new.png"));
    newAction->setShortcut(QKeySequence::New);
    newAction->setStatusTip("Создать новый документ");
    connect(newAction, SIGNAL(triggered()), this, SLOT(newDoc()));

    openAction = new QAction("&Открыть", this);
    openAction->setIcon(QIcon(":/icons/open.png"));
    openAction->setShortcut(QKeySequence::Open);
    openAction->setStatusTip("Открыть файл с исходным кодом");
    connect(openAction, SIGNAL(triggered()), this, SLOT(open()));

    saveAction = new QAction("&Сохранить", this);
    saveAction->setIcon(QIcon(":/icons/save.png"));
    saveAction->setShortcut(QKeySequence::Save);
    saveAction->setStatusTip("Сохранить исходный код");
    connect(saveAction, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAction = new QAction("Сохранить как", this);
    saveAsAction->setIcon(QIcon(":/icons/save-as.png"));
    saveAsAction->setShortcut(QKeySequence::SaveAs);
    saveAsAction->setStatusTip("Сохранить как");
    connect(saveAsAction, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAction = new QAction("&Выход", this);
    exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    exitAction->setStatusTip("Зыкрыть приложение");
    exitAction->setIcon(QIcon(":/icons/close.png"));
    exitAction->setMenuRole(QAction::QuitRole);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    undoAction = new QAction("Отменить", this);
    undoAction->setIcon(QIcon(":/icons/undo.png"));
    undoAction->setShortcut(QKeySequence::Back);
    undoAction->setStatusTip("Отменить");
    undoAction->setDisabled(true);
    connect(undoAction, SIGNAL(triggered()), codeEditor, SLOT(undo()));
    connect(codeEditor, SIGNAL(undoAvailable(bool)), undoAction, SLOT(setEnabled(bool)));

    redoAction = new QAction("Вернуть", this);
    redoAction->setIcon(QIcon(":/icons/redo.png"));
    redoAction->setShortcut(QKeySequence::Forward);
    redoAction->setStatusTip("Вернуть");
    redoAction->setDisabled(true);
    connect(redoAction, SIGNAL(triggered()), codeEditor, SLOT(redo()));
    connect(codeEditor, SIGNAL(redoAvailable(bool)), redoAction, SLOT(setEnabled(bool)));

    cutAction = new QAction("Вырезать", this);
    cutAction->setIcon(QIcon(":/icons/cut.png"));
    cutAction->setShortcut(QKeySequence::Cut);
    cutAction->setStatusTip("Вырезать");
    cutAction->setEnabled(false);
    connect(cutAction, SIGNAL(triggered()), codeEditor, SLOT(cut()));
    connect(codeEditor, SIGNAL(copyAvailable(bool)), cutAction, SLOT(setEnabled(bool)));

    copyAction = new QAction("Копировать", this);
    copyAction->setIcon(QIcon(":/icons/copy.png"));
    copyAction->setShortcut(QKeySequence::Copy);
    copyAction->setStatusTip("Копировать");
    copyAction->setEnabled(false);
    connect(copyAction, SIGNAL(triggered()), codeEditor, SLOT(copy()));
    connect(codeEditor, SIGNAL(copyAvailable(bool)), copyAction, SLOT(setEnabled(bool)));

    pasteAction = new QAction("Вставить", this);
    pasteAction->setIcon(QIcon(":/icons/paste.png"));
    pasteAction->setShortcut(QKeySequence::Paste);
    pasteAction->setStatusTip("Вставить");
    connect(pasteAction, SIGNAL(triggered()), codeEditor, SLOT(paste()));

    selectAllAction = new QAction("Выделить все", this);
    selectAllAction->setIcon(QIcon(":/icons/select-all.png"));
    selectAllAction->setShortcut(QKeySequence::SelectAll);
    selectAllAction->setStatusTip("Выделить все");
    connect(selectAllAction, SIGNAL(triggered()), codeEditor, SLOT(selectAll()));

    analyzeAction = new QAction("Анализ", this);
    analyzeAction->setIcon(QIcon(":/icons/do.png"));
    analyzeAction->setShortcut(QKeySequence("Ctrl+B"));
    analyzeAction->setStatusTip("Начать разбор");
    connect(analyzeAction, SIGNAL(triggered()), this, SLOT(analize()));

    analyzeAndRunAction = new QAction("Выполнить", this);
    analyzeAndRunAction->setIcon(QIcon(":/icons/do.png"));
    analyzeAndRunAction->setShortcut(QKeySequence("Ctrl+R"));
    analyzeAndRunAction->setStatusTip("Начать разбор и затем выполнить");
    connect(analyzeAndRunAction, SIGNAL(triggered()), this, SLOT(analizeAndRun()));

    stopExecAction = new QAction("Остановить", this);
    stopExecAction->setStatusTip("Остановить выполнение");
    connect(stopExecAction, SIGNAL(triggered()), this, SLOT(stopExec()));

    showGrammarAction = new QAction("Грамматика", this);
    showGrammarAction->setStatusTip("Показать грамматику");
    connect(showGrammarAction, SIGNAL(triggered()), this, SLOT(showGrammar()));

    showPriorytyTableAction = new QAction("Таблица приоритетов", this);
    showPriorytyTableAction->setStatusTip("Показать таблицу приоритетов");
    connect(showPriorytyTableAction, SIGNAL(triggered()), this, SLOT(showPriorytyTable()));

    showPrecedenceTableAction = new QAction("Таблица предшествований", this);
    showPrecedenceTableAction->setStatusTip("Показать таблицу предшествований");
    connect(showPrecedenceTableAction, SIGNAL(triggered()), this, SLOT(showPrecedenceTable()));

    showOutputAction = new QAction("Журнал восходящего разбора", this);
    showOutputAction->setStatusTip("Показать журнал восходящего разбора");
    connect(showOutputAction, SIGNAL(triggered()), this, SLOT(showOutput()));

    showPolisBuildAction = new QAction("Журнал построения ПОЛИЗа", this);
    showPolisBuildAction->setStatusTip("Показать журнал построения ПОЛИЗа");
    connect(showPolisBuildAction, SIGNAL(triggered()), this, SLOT(showPolisBuild()));

    showPolisAction = new QAction("ПОЛИЗ", this);
    showPolisAction->setStatusTip("Показать ПОЛИЗ");
    connect(showPolisAction, SIGNAL(triggered()), this, SLOT(showPolis()));

    aboutAction = new QAction("&О программе", this);
    aboutAction->setStatusTip("Показать окно справки");
    aboutAction->setIcon(QIcon(":/icons/about.png"));
    aboutAction->setStatusTip("О программе");
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    showPrecedenceTableAction->setDisabled(true);
    showOutputAction->setDisabled(true);
    showPolisBuildAction->setDisabled(true);
    showPolisAction->setDisabled(true);
    stopExecAction->setDisabled(true);

#ifdef Q_OS_MAC
    newAction->setIconVisibleInMenu(false);
    newAction->setIconVisibleInMenu(false);
    openAction->setIconVisibleInMenu(false);
    saveAction->setIconVisibleInMenu(false);
    saveAsAction->setIconVisibleInMenu(false);
    exitAction->setIconVisibleInMenu(false);
    aboutAction->setIconVisibleInMenu(false);
    analyzeAction->setIconVisibleInMenu(false);
    analyzeAndRunAction->setIconVisibleInMenu(false);
    undoAction->setIconVisibleInMenu(false);
    redoAction->setIconVisibleInMenu(false);
    cutAction->setIconVisibleInMenu(false);
    copyAction->setIconVisibleInMenu(false);
    pasteAction->setIconVisibleInMenu(false);
    selectAllAction->setIconVisibleInMenu(false);
#endif
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu("&Файл");
    fileMenu->addAction(newAction);
    fileMenu->addAction(openAction);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAction);
    fileMenu->addAction(saveAsAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    editMenu = menuBar()->addMenu("&Правка");
    editMenu->addAction(undoAction);
    editMenu->addAction(redoAction);
    editMenu->addSeparator();
    editMenu->addAction(cutAction);
    editMenu->addAction(copyAction);
    editMenu->addAction(pasteAction);
    editMenu->addSeparator();
    editMenu->addAction(selectAllAction);

    toolsMenu = menuBar()->addMenu("&Анализ");
    toolsMenu->addAction(analyzeAction);
    toolsMenu->addAction(analyzeAndRunAction);
    toolsMenu->addSeparator();
    toolsMenu->addAction(stopExecAction);
    toolsMenu->addSeparator();

    additionalToolsSubMenu = toolsMenu->addMenu("Дополнительно");

    toolsInfoSubMenu = additionalToolsSubMenu->addMenu("Информация");
    toolsInfoSubMenu->addAction(showGrammarAction);
    toolsInfoSubMenu->addAction(showPriorytyTableAction);

    resultInfoSubMenu = additionalToolsSubMenu->addMenu("Результат построения");
    resultInfoSubMenu->addAction(showPrecedenceTableAction);
    resultInfoSubMenu->addAction(showOutputAction);
    resultInfoSubMenu->addAction(showPolisBuildAction);
    resultInfoSubMenu->addAction(showPolisAction);

    aboutMenu = menuBar()->addMenu("&О программе");
    aboutMenu->addAction(aboutAction);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar("Файл");
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addAction(saveAction);

    editToolBar = addToolBar("Редактирование");
    editToolBar->addAction(cutAction);
    editToolBar->addAction(copyAction);
    editToolBar->addAction(pasteAction);
    editToolBar->addSeparator();
    editToolBar->addAction(undoAction);
    editToolBar->addAction(redoAction);

    doToolBar = addToolBar("Анализ");
    doToolBar->addAction(analyzeAction);

    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::setupUi()
{
    codeEditor = new CodeEditor;
    codeEditor->setMinimumHeight(200);
    codeEditor->setMinimumWidth(400);

    msgsListWidget = new QListWidget;
    errorsListWidget = new QListWidget;
    warningsListWidget = new QListWidget;

    console = new Console;

    outputTabWidget = new QTabWidget;
    outputTabWidget->addTab(msgsListWidget, QIcon(":/icons/msg.png"),"Сообщения");
    outputTabWidget->addTab(errorsListWidget, QIcon(":/icons/error.png"), "Ошибки");
    outputTabWidget->addTab(warningsListWidget, QIcon(":/icons/warning.png"), "Предупреждения");
    outputTabWidget->addTab(console, QIcon(":/icons/terminal.png"), "Консоль приложения");
    outputTabWidget->setTabPosition(QTabWidget::North);
    outputTabWidget->setTabShape(QTabWidget::Rounded);

    QSplitter *mainSplitter = new QSplitter(this);
    mainSplitter->setOrientation(Qt::Vertical);
    mainSplitter->addWidget(codeEditor);
    mainSplitter->addWidget(outputTabWidget);
    mainSplitter->setStretchFactor(0, 6);
    mainSplitter->setStretchFactor(1, 1);

    mainSplitter->setMinimumWidth(600);

    setCentralWidget(mainSplitter);

    setWindowTitle("Транслятор pro");
}

void MainWindow::open()
{
    if(okToContinue()) {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Открыть файл с исходным кодом"), QDir::homePath(), tr("Файлы исходного кода (*.src *.txt *.prg)"));
        if(fileName != NULL) {
            QFile file(fileName);
            if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qDebug() << "Не удалось открыть файл на чтение;(";
                return;
            }
            fName = fileName;
            QString data;
            data = file.readAll();
            codeEditor->setPlainText(data);

            file.close();
            codeEditor->document()->setModified(false);
        }
    }
}

bool MainWindow::save()
{
    if(fName.isEmpty()) {
        return saveAs();
    }

    QFile file(fName);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "Не удалось открыть файл на запись;(";
        return false;
    }
    QTextStream out(&file);//поток для записи текста
    out << codeEditor->toPlainText();
    file.close();
    codeEditor->document()->setModified(false);
    return true;
}

bool MainWindow::saveAs()
{
    QString fileStr;
    if(fName.isEmpty()) {
        fileStr = QDir::homePath();
        fileStr += "/sources.src";
    }
    else {
        fileStr = fName;
    }
    QString fileName = QFileDialog::getSaveFileName(this, tr("Сохранить исходный код"),
                                                    fileStr, tr("Файлы исходного кода (*.src)"));
    if(!fileName.isEmpty()) {
        fName = fileName;
        return save();
    }
    return false;
}

void MainWindow::newDoc()
{
    if(okToContinue()) {
        codeEditor->clear();
        codeEditor->document()->setModified(false);
        //setWindowFilePath("Новенький.src");
        //setWindowModified(ture);
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("Про транслятор"),
                       tr("<h2>Транслятор pro</h2>"
                          "<p>Copyright &copy; 2013 bo-nik corp."
                          "<p>Эта, с позволения сказать, программа была написана в учебных целях и от нечего делать. "
                          "Больше, конечно, от нечего делать."
                          "<p>Написано на "));
}

bool MainWindow::okToContinue()
{
    if (codeEditor->document()->isModified()) {
        int r = QMessageBox::question(this, tr("Анализатор"),
                                      tr("Исходный код был изменен.\n"
                                         "Сохранить изменения?"),
                                      tr("Да"), tr("Нет"), tr("Отмена"));
        if (r == 0) {
            return save();
        }
        else if (r == 2) {
            return false;
        }
    }
    return true;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (okToContinue()) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

void MainWindow::analize()
{
    analyzed = false;

    analyzeAction->setDisabled(true);
    analyzeAndRunAction->setDisabled(true);

    errorsLines.clear();
    warningsLines.clear();

    msgsListWidget->clear();
    errorsListWidget->clear();
    warningsListWidget->clear();

    updateErrorsCount();
    updateWarningsCount();

    analyzer->setSrcCode(codeEditor->toPlainText());

    analyzer->start();
}

void MainWindow::analizeAndRun()
{
    analyzeAndRun = true;
    analize();
}

void MainWindow::evaluate()
{
    //тут будет выполнение
    evaluator->setInputCode(analyzer->getPolis());
    evaluator->clearVTable();
    stopExecAction->setEnabled(true);
    for (int i = 0; i < analyzer->getVTable().size(); i++) {
        evaluator->addVarible(analyzer->getVTable()[i].toStdString());
    }

    console->startExec();
    infoMsg("Выполнение начато в " +
            QTime::currentTime().toString() + " " +
            QDate::currentDate().toString(Qt::DefaultLocaleShortDate),
            Qt::black);
    evaluator->start();
}

void MainWindow::analyzeFinished()
{
    updateErrorsCount();
    updateWarningsCount();
    analyzed = true;
    showPrecedenceTableAction->setEnabled(true);
    showOutputAction->setEnabled(true);
    showPolisBuildAction->setEnabled(true);
    showPolisAction->setEnabled(true);

    if (!analyzer->isSuccessful()) {
        analyzeAction->setEnabled(true);
        analyzeAndRunAction->setEnabled(true);
        return;
    }

    if (analyzeAndRun) {
        analyzeAndRun = false;
        evaluate();
    }
    else {
        analyzeAction->setEnabled(true);
        analyzeAndRunAction->setEnabled(true);
    }
}

void MainWindow::evaluateFinished()
{
    analyzeAction->setEnabled(true);
    analyzeAndRunAction->setEnabled(true);
    stopExecAction->setDisabled(true);

    if (!evaluator->isSuccessful()) {
        console->finishExecWithErrors();
        infoMsg("Выполнение аварийно завершено в " +
                QTime::currentTime().toString() + " " +
                QDate::currentDate().toString(Qt::DefaultLocaleShortDate),
                Qt::black);
        return;
    }
    console->finishExec();
    infoMsg("Выполнение завершено в " +
            QTime::currentTime().toString() + " " +
            QDate::currentDate().toString(Qt::DefaultLocaleShortDate),
            Qt::black);
}

void MainWindow::stopExec()
{
    evaluator->terminate();
}

void MainWindow::needVarValueToRead(QString promt)
{
    console->getValue(promt);
}

void MainWindow::cmdFromConsole(QString cmd)
{
    //evaluator->setIdValueForRead(cmd.toDouble());
    emit idForRead(cmd.toDouble());
}

void MainWindow::infoMsg(QString msg, QColor color)
{
    QListWidgetItem *i = new QListWidgetItem(msg);
    i->setTextColor(color);
    msgsListWidget->addItem(i);
    msgsListWidget->scrollToBottom();
}

void MainWindow::errorMsg(int line, QString msg, QColor color)
{
    QListWidgetItem *i = new QListWidgetItem(QString(tr("Строка %1:   %2").arg(line).arg(msg)));
    i->setTextColor(color);
    i->setIcon(QIcon(":/icons/error.png"));
    errorsListWidget->addItem(i);
    errorsLines.push_back(line-1);

    updateErrorsCount();
}

void MainWindow::warningMsg(int line, QString msg, QColor color)
{
    QListWidgetItem *i = new QListWidgetItem(QString(tr("Строка %1:   %2").arg(line).arg(msg)));
    i->setTextColor(color);
    i->setIcon(QIcon(":/icons/warning.png"));
    warningsListWidget->addItem(i);
    warningsLines.push_back(line-1);

    updateWarningsCount();
}

void MainWindow::programMsg(QString msg)
{
    console->output(msg);
}

void MainWindow::updateErrorsCount()
{
    if(errorsListWidget->count())
        outputTabWidget->setTabText(1, QString(tr("Ошибки (%1)").arg(errorsListWidget->count())));
    else
        outputTabWidget->setTabText(1, QString(tr("Ошибки")));
}

void MainWindow::updateWarningsCount()
{
    if(warningsListWidget->count())
        outputTabWidget->setTabText(2, QString(tr("Предупреждения (%1)").arg(warningsListWidget->count())));
    else
        outputTabWidget->setTabText(2, QString(tr("Предупреждения")));
}

void MainWindow::errorItemClicked(QListWidgetItem *item)
{
    if(errorsListWidget->row(item) < 0)
        return;
    QTextCursor cur(codeEditor->document());
    cur.setPosition(0, QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, errorsLines[errorsListWidget->row(item)]);
    codeEditor->setTextCursor(cur);
}

void MainWindow::warningItemClicked(QListWidgetItem *item)
{
    if(warningsListWidget->row(item) < 0)
        return;
    QTextCursor cur(codeEditor->document());
    cur.setPosition(0, QTextCursor::MoveAnchor);
    cur.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor, warningsLines[warningsListWidget->row(item)]);
    codeEditor->setTextCursor(cur);
}

//tools
void MainWindow::showGrammar()
{
    grammarDialog->show();
}

void MainWindow::showPriorytyTable()
{
    priorityShowDialog->show();
}

void MainWindow::showPrecedenceTable()
{
    precedenceTableDialog->setSize(analyzer->getGrammarSize());
    precedenceTableDialog->setGrammar(analyzer->getGrammarSymbols());
    precedenceTableDialog->setTable(analyzer->getPrecTable());
    precedenceTableDialog->show();
}

void MainWindow::showOutput()
{
    outputDialog->setData(analyzer->getAnalyzeProcess());
    outputDialog->show();
}

void MainWindow::showPolisBuild()
{
    polisBuildDialog->setData(analyzer->getAnalyzePolisProcess());
    polisBuildDialog->show();
}

void MainWindow::showPolis()
{
    polisShowDialog->setData(analyzer->getPolis());
    polisShowDialog->show();
}
