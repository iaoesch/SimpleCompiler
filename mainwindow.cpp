#include "mainwindow.h"
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QPainter>
#include <QTextEdit>
#include <QSvgWidget>
#include <sstream>
#include <fstream>

#include "driver.hh"
#include "highlighter.h"
#include "compact.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *widget = new QWidget();

    setupEditor();

    QVBoxLayout *VLayout = new QVBoxLayout;
    widget->setLayout(VLayout);
    setCentralWidget(widget);

    QHBoxLayout *HLayout1 = new QHBoxLayout;
    QHBoxLayout *HLayout2 = new QHBoxLayout;
    QLabel *TopLabel = new QLabel("L1");
    VLayout->addWidget(TopLabel);
    VLayout->addLayout(HLayout1);
    VLayout->addLayout(HLayout2);
    SvgDisplay = new QSvgWidget();
    QLabel *BottomLabel = new QLabel();
    VLayout->addWidget(BottomLabel);
    VLayout->addWidget(SvgDisplay);


    HLayout1->addWidget(editor);
    Output = new QLabel("some results");
    HLayout1->addWidget(Output);

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
        TreeToSVG(drv.result, "tree.dot", "tree.svg");
        return Output.str();
}

void MainWindow::TreeToSVG(std::list<std::shared_ptr<StatementClass>> Graph, std::string DotFilePath, std::string SVGFilePath)
{

    //if (Graph.empty()) {
    std::ofstream Drawing(DotFilePath);
    Drawing << "digraph g {" << std::endl;
    Drawing << "node [shape = record,height=.1];" << std::endl;
    for (auto &s: Graph) {
        if (s!=nullptr)  {
            s->DrawNode(Drawing, 0);
        }
    }
    Drawing << "}" << std::endl;
    Drawing.close();

    system(("/opt/homebrew/bin/dot -Tsvg " + DotFilePath + " -o " + SVGFilePath).c_str());
    //system("open tree2.png");

    SvgDisplay->load(QString::fromStdString(SVGFilePath));

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


