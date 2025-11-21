#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QPushButton>
#include <QToolButton>
#include <QMainWindow>
#include <QLabel>

#include "driver.hh"
#include "environment.hpp"
#include "highlighter.h"

QT_BEGIN_NAMESPACE
class QTextEdit;
class QLabel;
class QSvgWidget;
QT_END_NAMESPACE
class StatementClass;
class MainWindow;

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



class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef QToolButton ButtonType;

    ButtonType *Buttons[3][3];
    QLabel *TopLabel;
    QLabel *BottomLabel;
    QPixmap *Cross;
    QPixmap *Circle;
    QPushButton *Stop;
    bool Stoprequest;

    QtEnvironment Env;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void TextChanged();
    void StopButtonClicked();

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

     std::tuple<std::string, driver::ErrorListType> ParseBlock(std::string Codeblock);
     void UnMarkDocument();
};
#endif // MAINWINDOW_H

