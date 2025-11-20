#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>

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

    std::string ParseBlock(std::string Codeblock);
    void MarkRange(int StartLine, int StartColumn, int EndLine, int EndColumn);
    void TreeToSVG(std::list<std::shared_ptr<StatementClass> > Graph, std::string DotFilePath, std::string SVGFilePath);

    friend class QtEnvironment;
    bool CheckForStop() ;
     void ExecutionStarted() ;
     void ExecutionStopped() ;

};
#endif // MAINWINDOW_H

