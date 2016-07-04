#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>

#include "code_editor/codeeditor.h"
#include "analyzer/analyzer.h"
#include "analyzer/evaluator.h"
#include "console/console.h"

#include "tools/grammardialog.h"
#include "tools/priorityshowdialog.h"
#include "tools/precedencetabledialog.h"
#include "tools/outputdialog.h"
#include "tools/polisbuilddialog.h"
#include "tools/polisshowdialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void setupUi();
    bool okToContinue();

    void updateErrorsCount();
    void updateWarningsCount();

private slots:
    void open();
    bool save();
    bool saveAs();
    void newDoc();
    void about();
    void analize();
    void analizeAndRun();
    void stopExec();
    void evaluate();
    void needVarValueToRead(QString promt);
    void cmdFromConsole(QString cmd);
    void showGrammar();
    void showPriorytyTable();
    void showPrecedenceTable();
    void showOutput();
    void showPolisBuild();
    void showPolis();

    void infoMsg(QString msg, QColor color);
    void errorMsg(int line, QString msg, QColor color);
    void warningMsg(int line, QString msg, QColor color);
    void programMsg(QString msg);

    void analyzeFinished();
    void evaluateFinished();

    void errorItemClicked(QListWidgetItem * item);
    void warningItemClicked(QListWidgetItem * item);

signals:
    void idForRead(double val);

private:
    //widgets
    CodeEditor *codeEditor;
    QTabWidget *outputTabWidget;
    QListWidget *msgsListWidget;
    QListWidget *errorsListWidget;
    QListWidget *warningsListWidget;
    Console *console;

    //menus
    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *toolsMenu;
    QMenu *additionalToolsSubMenu;
    QMenu *toolsInfoSubMenu;
    QMenu *resultInfoSubMenu;
    QMenu *aboutMenu;

    //toolbars
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *doToolBar;

    //actions
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *saveAsAction;
    QAction *exitAction;
    QAction *aboutAction;

    QAction *analyzeAction;
    QAction *analyzeAndRunAction;
    QAction *stopExecAction;

    QAction *showGrammarAction;
    QAction *showPriorytyTableAction;
    QAction *showPrecedenceTableAction;
    QAction *showOutputAction;
    QAction *showPolisBuildAction;
    QAction *showPolisAction;

    QAction *undoAction;
    QAction *redoAction;
    QAction *cutAction;
    QAction *copyAction;
    QAction *pasteAction;
    QAction *selectAllAction;

    GrammarDialog *grammarDialog;
    PriorityShowDialog *priorityShowDialog;
    PrecedenceTableDialog *precedenceTableDialog;\
    OutputDialog *outputDialog;
    PolisBuildDialog *polisBuildDialog;
    PolisShowDialog *polisShowDialog;

private:
    QString fName;

private:
    Analyzer *analyzer;
    Evaluator *evaluator;

    QList<int> errorsLines;
    QList<int> warningsLines;

    bool analyzed;
    bool analyzeAndRun;

};

#endif // MAINWINDOW_H
