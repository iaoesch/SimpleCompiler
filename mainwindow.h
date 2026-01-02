#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPushButton>
#include <QToolButton>
#include <QMainWindow>
#include <QLabel>

#include "driver.hh"
#include "environment.hpp"
#include "highlighter.h"
#include "inputdialogclass.h"

QT_BEGIN_NAMESPACE
class QTextEdit;
class QLabel;
class QSvgWidget;
QT_END_NAMESPACE
class StatementClass;
class MainWindow;
class SystemInterfaceClass;

class QtEnvironment : public Environment {

    MainWindow &Parent;

public:
    explicit QtEnvironment(MainWindow &Parent) : Parent(Parent) {}
    // Environment interface

    virtual std::ostream &OutputStream() override;
    virtual std::istream &InputStream() override;
    virtual bool CheckForStop() override;
    virtual void ExecutionStarted() override;
    virtual void ExecutionStopped() override;
};

using namespace std::chrono_literals;

class QtTestEnvironment : public Environment {

    MainWindow &Parent;
    std::ostringstream CapturedOutputStream;
    std::istringstream TestInputStream;
    std::chrono::milliseconds TimeoutTime;

public:
    explicit QtTestEnvironment(MainWindow &Parent, const std::string &TestInput) : Parent(Parent), TestInputStream(TestInput), TimeoutTime(1000ms) {}
    // Environment interface

    virtual std::ostream &OutputStream() override;
    virtual std::istream &InputStream() override;
    virtual bool CheckForStop() override;
    virtual void ExecutionStarted() override;
    virtual void ExecutionStopped() override;

    void SetTimeout(std::chrono::milliseconds TimeoutTime_) {TimeoutTime = TimeoutTime_;}
    std::string GetCapturedOutput() const {return CapturedOutputStream.str();}
};



class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef QToolButton ButtonType;

    ButtonType *Buttons[3][3];
    QLabel *TopLabel;
    QLabel *BottomLabel;
    QCheckBox *DebugMode;
    QPixmap *Cross;
    QPixmap *Circle;
    QPushButton *Stop;
    QPushButton *Run;
    QPushButton *RunTests;
    bool Stoprequest;

    QTimer *TestTimer;

    QtEnvironment Env;

    QString Documentpath;

    std::unique_ptr<driver> CurrentCode;

    SystemInterfaceClass *SystemInterface;

public slots:
    void about();
    void newFile();
    void openFile(QString path = QString());
    void saveFile(QString path = QString());
    void ShowInputDialog(InputDialogClass::DialogDescriptor *Descriptor, std::vector<InputDialogClass::ValueType> *Result);


private:
    void setupFileMenu();
    void setupHelpMenu();
    bool userReallyWantsToQuit() {return true;}

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    std::vector<std::string> DoOneTest(std::string Codeblock, std::map<std::string, GlobalVariableClass> Expected);
public slots:
    void TextChanged();
    void StopButtonClicked();
    void RunButtonClicked();
    void RunTestButtonClicked();

protected:
    void closeEvent(QCloseEvent *event) override;
private:
    void setupEditor();

    QTextEdit *editor;
    Highlighter *highlighter;
    QLabel *Output;
    QSvgWidget *SvgDisplay;
    int ChangingInProgress;

    void MarkRange(yy::location Location, const std::string &Messge);
    void TreeToSVG(std::list<std::shared_ptr<StatementClass> > Graph, std::string DotFilePath, std::string SVGFilePath);

    friend class QtEnvironment;
    bool CheckForStop() ;
     void ExecutionStarted() ;
     void ExecutionStopped() ;

     friend class QtTestEnvironment;
     bool CheckForTestTimeout() ;
     void ExecutionStartTest(std::chrono::milliseconds TimeoutTime) ;
     void ExecutionStoppTest() ;
     bool TestTimeoutOccoured;
     private slots:
     void TestTimeRunOut();

private:
     std::tuple<std::string, driver::ErrorListType> ParseBlock(std::string Codeblock);
     void UnMarkDocument();
};
#endif // MAINWINDOW_H

