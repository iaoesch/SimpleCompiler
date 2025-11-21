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
#include <QApplication>

#include "driver.hh"
#include "highlighter.h"
#include "compact.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), Env(*this)
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

    Stop = new QPushButton("Stopped");
    Stop->setDisabled(true);
    Stoprequest = false;
    connect(Stop, &QPushButton::clicked, this, &MainWindow::StopButtonClicked);
    HLayout2->addWidget(Stop);

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
        auto[Result, Errors] = ParseBlock(Code.toStdString());
        Output->setText(QString::fromStdString(Result));
        UnMarkDocument();
        for (auto const &e: Errors) {
            std::cout << "Marking: " << e.Location << std::endl;
           MarkRange(e.Location, e.Message);
        }
        ChangingInProgress--;
    }
}

void MainWindow::StopButtonClicked()
{
    Stoprequest = true;
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


void QtEnvironment::ExecutionStarted()
{
    Parent.ExecutionStarted();
}

void QtEnvironment::ExecutionStopped()
{
    Parent.ExecutionStopped();
}

std::ostream &QtEnvironment::OutputStream()
{
    return std::cout;
}

std::istream &QtEnvironment::InputStream()
{
    return std::cin;
}

bool QtEnvironment::CheckForStop()
{
    return Parent.CheckForStop();
}

std::tuple<std::string, driver::ErrorListType> MainWindow::ParseBlock (std::string Codeblock)
{
    int res = 0;
    driver drv(Env);
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
        return {Output.str(), drv.GetErrors()};
}

void MainWindow::TreeToSVG(std::list<std::shared_ptr<StatementClass>> Graph, std::string DotFilePath, std::string SVGFilePath)
{

    //if (Graph.empty()) {
    RestartNodeNumber();
    std::ofstream Drawing(DotFilePath);
    Drawing << "digraph g {" << std::endl;
    Drawing << "node [shape = record,height=.1];" << std::endl;
    int CurrentNodeNumber = GetNextNodeNumber();
    Drawing << "Node" << CurrentNodeNumber << "[label = \"<f0> |<f1> Start |<f2> \"];" << std::endl;

    for (auto &s: Graph) {
        if (s!=nullptr)  {
            int  NextNodeNumber = GetNextNodeNumber();
            Drawing << "\"Node" << CurrentNodeNumber << "\":f0 -> \"Node" << NextNodeNumber << "\":f1;" << std::endl;

            s->DrawNode(Drawing, NextNodeNumber);
            CurrentNodeNumber = NextNodeNumber;
        }
    }
    Drawing << "}" << std::endl;
    Drawing.close();

    system(("/opt/homebrew/bin/dot -Tsvg " + DotFilePath + " -o " + SVGFilePath).c_str());
    //system("open tree2.png");

    SvgDisplay->load(QString::fromStdString(SVGFilePath));

}

bool MainWindow::CheckForStop()
{
    QApplication::processEvents();
    return Stoprequest;

}

void MainWindow::ExecutionStarted()
{
    Stoprequest = false;
    Stop->setText("Stop");
    Stop->setDisabled(false);
}

void MainWindow::ExecutionStopped()
{
    Stoprequest = false;
    Stop->setText("Stoped");
    Stop->setDisabled(true);

}

void MainWindow::UnMarkDocument()
{
    QTextDocument *doc = editor->document();
    QTextCursor Cursor(doc);

    // Remove all underlines (Might be done more effizient
    // by just remove last set format)
    Cursor.movePosition(QTextCursor::Start);
    Cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    QTextCharFormat Format1;
    Format1.setUnderlineStyle(QTextCharFormat::NoUnderline);
    Format1.setFontUnderline(false);
    if (Cursor.hasSelection()) {
        auto CurrentFormat = editor->currentCharFormat();
        Cursor.mergeCharFormat(Format1);
        editor->setCurrentCharFormat(CurrentFormat);
    }
}

void MainWindow::MarkRange(yy::location Location, const std::string &Message)
{
    QTextDocument *doc = editor->document();
    QTextCursor Cursor(doc);


    if (Location.begin.line > 1) {
       if (Cursor.movePosition(QTextCursor::NextBlock, QTextCursor::MoveAnchor,
                               Location.begin.line-1) == false) {return;}
    }
    if (Cursor.block().length() >= Location.begin.column) {
        if (Location.begin.column > 2) {
            Location.begin.column-=2;
        } else {
            Location.begin.column = 0;
        }
    }
    if (Location.begin.column > 1) {
    if (Cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor,
                        Location.begin.column-1) == false) {return;}
    }
    if (Cursor.atBlockEnd()) {
        Cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor);
    }
    Cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor,
                        Location.end.line - Location.begin.line);
    Cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor,
                        Location.end.column - Location.begin.column);
    QTextCharFormat Format;
    Format.setUnderlineStyle(QTextCharFormat::WaveUnderline);
    Format.setUnderlineColor(Qt::red);
    Format.setToolTip(QString::fromStdString(Message));
    Format.setFontUnderline(true);

    if (Cursor.hasSelection()) {
        auto CurrentFormat = editor->currentCharFormat();
        Cursor.mergeCharFormat(Format);
        editor->setCurrentCharFormat(CurrentFormat);
    }

}


