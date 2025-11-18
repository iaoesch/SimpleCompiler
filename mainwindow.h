#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>

#include "highlighter.h"

QT_BEGIN_NAMESPACE
class QTextEdit;
class QLabel;
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

    typedef QToolButton ButtonType;

    ButtonType *Buttons[3][3];
    QLabel *TopLabel;
    QLabel *BottomLabel;
    QPixmap *Cross;
    QPixmap *Circle;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void TextChanged();

private:
    void setupEditor();

    QTextEdit *editor;
    Highlighter *highlighter;
    QLabel *Output;
    int ChangingInProgress;

    std::string ParseBlock(std::string Codeblock);
    void MarkRange(int StartLine, int StartColumn, int EndLine, int EndColumn);
};
#endif // MAINWINDOW_H

