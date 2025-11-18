#include "mainwindow.h"
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QPainter>
#include <QTextEdit>
#include <sstream>

#include "driver.hh"
#include "highlighter.h"
#include "compact.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *widget = new QWidget();

    setupEditor();

    QHBoxLayout *VLayout = new QHBoxLayout;
    widget->setLayout(VLayout);
    setCentralWidget(widget);

    QVBoxLayout *HLayout1 = new QVBoxLayout;
    QVBoxLayout *HLayout2 = new QVBoxLayout;
    QLabel *TopLabel = new QLabel("L1");
    //VLayout->addWidget(TopLabel);
    VLayout->addLayout(HLayout1);
    VLayout->addLayout(HLayout2);
    QLabel *BottomLabel = new QLabel();
    VLayout->addWidget(BottomLabel);

    HLayout1->addWidget(editor);
    Output = new QLabel("some results");
    HLayout2->addWidget(Output);

    connect(editor, &QTextEdit::textChanged, this, &MainWindow::TextChanged);
    ChangingInProgress = 0;
#if 0
    Cross = new QPixmap(30,30);
    QPainter p(Cross);
    p.fillRect(0,0,29,29,QBrush(Qt::white));
    p.drawLine(2,2,28,28);
    p.drawLine(2,28,28,2);

    Circle = new QPixmap(30,30);
    QPainter p2(Circle);
    p2.fillRect(0,0,29,29,QBrush(Qt::white));
    p2.setPen(QPen(QBrush(Qt::black),3));
    p2.drawEllipse(2,2,28,28);

    for (int i = 0; i < 3; i++) {
        QVBoxLayout *GridVLayout = new QVBoxLayout;
        HLayout->addLayout(GridVLayout);
        for (int j = 0; j < 3; j++) {
            ButtonType *Button = new ButtonType();
            //Button.se
            Button->setIcon(*Circle);
            connect(Button, &ButtonType::clicked, this, &MainWindow::ButtonPressed);
            Buttons[i][j] = Button;
            GridVLayout->addWidget(Button);
        }
    }
#endif

}



MainWindow::~MainWindow()
{
}

void MainWindow::TextChanged()
{
    if (ChangingInProgress == 0) {
        ChangingInProgress++;

    QTextEdit* buttonSender = qobject_cast<QTextEdit*>(sender()); // retrieve the button you have clicked
    QString Code = buttonSender->toPlainText();
    std::string Result = ParseBlock(Code.toStdString());
    Output->setText(QString::fromStdString(Result));
    MarkRange(2,4,2,14);
    ChangingInProgress--;
    }
}

void MainWindow::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    editor = new QTextEdit;
    editor->setFont(font);
    editor->setWordWrapMode(QTextOption::NoWrap);

    highlighter = new Highlighter(editor->document());

//    QFile file("mainwindow.h");
//    if (file.open(QFile::ReadOnly | QFile::Text))
//        editor->setPlainText(file.readAll());
}



std::string MainWindow::ParseBlock (std::string Codeblock)
{
    int res = 0;
    driver drv;
    drv.result.clear();
    try {
        if (drv.parse(Codeblock.c_str())) {
                        std::cout << "Abnormal parsing end" << std::endl;
        }
        } catch (ErrorBaseClass &e) {
                    std::cout << "Exception: " << e.what() << std::endl;
                    std::cout << drv.location << std::endl;
        } catch (std::exception &e) {
                    std::cout << "Exception: " << e.what() << std::endl;
                    std::cout << drv.location << std::endl;
        } catch (...) {
                    std::cout << "Unknown Exception: " << std::endl;
                    std::cout << drv.location << std::endl;

        }
        std::ostringstream Output;
        if (drv.result.empty()) {
                    Output << "<empty list>" << std::endl;
        }
        for (auto &s: drv.result) {
            s->Print(Output);
        }
        drv.Variables.Dump(Output);
        return Output.str();
}

void MainWindow::MarkRange(int StartLine, int StartColumn, int EndLine, int EndColumn)
{
    QTextDocument *doc = editor->document();
    QTextCursor Cursor(doc);
  //  Cursor.movePosition(QTextCursor::Start);
    Cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,
                        StartLine);
    Cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor,
                        StartColumn);
    Cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor,
                        EndLine - StartLine);
    Cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor,
                        EndColumn - StartColumn);
    QTextCharFormat Format;
    Format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    Format.setUnderlineColor(Qt::red);
    Format.setFontUnderline(true);
    if (Cursor.hasSelection()) {
        Cursor.mergeCharFormat(Format);
    }
}


