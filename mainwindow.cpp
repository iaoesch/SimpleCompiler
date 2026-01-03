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
#include <QSplitter>
#include <QMenu>
#include <QMenuBar>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QSettings>
#include <QCheckBox>
#include <QTimer>
#include <QScrollArea>

#include "driver.hh"
#include "highlighter.h"
#include "compact.h"
#include "inputdialogclass.h"
#include "systeminterfaceclass.h"
#include "testclass.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), Env(*this)
{
    QSettings settings("OsiSoft", "Compiler Experiments");
    settings.beginGroup("mainwindow");
    Documentpath = settings.value("Last Path", QStandardPaths::displayName(QStandardPaths::DocumentsLocation)).toString();
    settings.endGroup();


    //QWidget *widget = new QWidget();
    QSplitter *Splitter = new QSplitter;
    Splitter->setOrientation(Qt::Vertical);
    setupEditor();
    setupFileMenu();
    setupHelpMenu();

    //QVBoxLayout *VLayout = new QVBoxLayout;
    //widget->setLayout(VLayout);
    //setCentralWidget(widget);
    setCentralWidget(Splitter);

    QHBoxLayout *HLayout1 = new QHBoxLayout;
    QHBoxLayout *HLayout2 = new QHBoxLayout;
    QLabel *TopLabel = new QLabel("L1");
    Splitter->addWidget(TopLabel);
    QWidget *widget = new QWidget();
    widget->setLayout(HLayout1);
    Splitter->addWidget(widget);
    widget = new QWidget();
    widget->setLayout(HLayout2);
    Splitter->addWidget(widget);
//    VLayout->addLayout(HLayout1);
//    VLayout->addLayout(HLayout2);
    SvgDisplay = new QSvgWidget();
    QLabel *BottomLabel = new QLabel();
    Splitter->addWidget(BottomLabel);
    Splitter->addWidget(SvgDisplay);


    HLayout1->addWidget(editor);
    Output = new QLabel("some results");
    QScrollArea *Scroller = new QScrollArea();
    Scroller->setWidget(Output);
    Scroller->setWidgetResizable(true);
    HLayout1->addWidget(Scroller);

    Stop = new QPushButton("Stopped");
    Stop->setDisabled(true);
    Stoprequest = false;
    connect(Stop, &QPushButton::clicked, this, &MainWindow::StopButtonClicked);
    HLayout2->addWidget(Stop);

    Run = new QPushButton("Run");
    Run->setDisabled(false);
    connect(Run, &QPushButton::clicked, this, &MainWindow::RunButtonClicked);
    HLayout2->addWidget(Run);

    RunTests = new QPushButton("Run Tests");
    connect(RunTests, &QPushButton::clicked, this, &MainWindow::RunTestButtonClicked);
    HLayout2->addWidget(RunTests);

    DebugMode = new QCheckBox("Debug");
    HLayout2->addWidget(DebugMode);

    connect(editor, &QTextEdit::textChanged, this, &MainWindow::TextChanged);
    ChangingInProgress = 0;

    SystemInterface = new(SystemInterfaceClass);
    connect(SystemInterface, &SystemInterfaceClass::ShowInputDialog, this, &MainWindow::ShowInputDialog);

    TestTimer = new QTimer;
    TestTimer->setSingleShot(true);
    connect(TestTimer, &QTimer::timeout, this, &MainWindow::TestTimeRunOut);

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

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (userReallyWantsToQuit()) {
        QSettings settings("OsiSoft", "Compiler Experiments");
        settings.beginGroup("mainwindow");
        settings.setValue("Last Path", Documentpath);
        settings.endGroup();
        event->accept();
    } else {
        event->ignore();
    }
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
            std::cout << "premarking: " << e.Location.begin.column << e.Location.begin.line
                      << e.Location.end.column << e.Location.end.line << "\n";
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


void MainWindow::RunButtonClicked()
{

    if (CurrentCode != nullptr) {
        Run->setDisabled(true);
        CurrentCode->Run();
        Run->setDisabled(false);
    }
}

void MainWindow::RunTestButtonClicked()
{
    QtTestEnvironment TestEnv(*this, std::string());
    TestClass Tests(TestEnv, SystemInterface);
    std::vector<std::string> FailedCodeblocks = Tests.DoAllTests();
    if (FailedCodeblocks.size() > 0) {

        editor->setText(QString::fromStdString(FailedCodeblocks.back()));
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


void QtTestEnvironment::ExecutionStarted()
{
    Parent.ExecutionStartTest(TimeoutTime);
}

void QtTestEnvironment::ExecutionStopped()
{
    Parent.ExecutionStoppTest();
}

std::ostream &QtTestEnvironment::OutputStream()
{
    return CapturedOutputStream;
}

std::istream &QtTestEnvironment::InputStream()
{
    return TestInputStream;
}

bool QtTestEnvironment::CheckForStop()
{
    return Parent.CheckForTestTimeout();
}

std::tuple<std::string, driver::ErrorListType> MainWindow::ParseBlock (std::string Codeblock)
{
    //int res = 0;
    std::unique_ptr<driver> drv = std::make_unique<driver>(Env, SystemInterface);
    drv->SetParserDebugLevel(DebugMode->isChecked()?1:0);
    drv->result.clear();
    try {
        if (drv->parse(Codeblock.c_str())) {
                        std::cout << "Abnormal parsing end" << std::endl;
        }
        } catch (ErrorBaseClass &e) {
                    std::cout << "Exception: " << e.what() << std::endl;
                    std::cout << drv->location << std::endl;
        } catch (std::exception &e) {
                    std::cout << "Exception: " << e.what() << std::endl;
                    std::cout << drv->location << std::endl;
        } catch (...) {
                    std::cout << "Unknown Exception: " << std::endl;
                    std::cout << drv->location << std::endl;

        }
        std::ostringstream Output;
        if (drv->result.empty()) {
                    Output << "<empty list>" << std::endl;
        }
        for (auto &s: drv->result) {
            s->Print(Output);
        }
        drv->Variables.Dump(Output);
        TreeToSVG(drv->result, "tree.dot", "tree.svg");
        CurrentCode = std::move(drv);
        return {Output.str(), CurrentCode->GetErrors()};
}

std::vector<std::string> MainWindow::DoOneTest (std::string Codeblock, std::map<std::string, GlobalVariableClass> Expected)
{
    std::vector<std::string> Errors;
    std::unique_ptr<driver> drv = std::make_unique<driver>(Env, SystemInterface);
    drv->SetParserDebugLevel(0);
    drv->result.clear();
    try {
        if (drv->parse(Codeblock.c_str())) {
            Errors.push_back("Abnormal parsing end");
            return Errors;
        }
    } catch (ErrorBaseClass &e) {
        std::ostringstream os;
        os << " at Location: " << drv->location;
        Errors.push_back(std::string("Exception: ") + e.what() + os.str());
        return Errors;
    } catch (std::exception &e) {
        std::ostringstream os;
        os << " at Location: " << drv->location;
        Errors.push_back(std::string("Exception: ") + e.what() + os.str());
        return Errors;
    } catch (...) {
        std::ostringstream os;
        os << " at Location: " << drv->location;
        Errors.push_back(std::string("Unknown Exception ") + os.str());
        return Errors;
    }
    drv->Run();
    for (auto &v: Expected) {
        std::shared_ptr<VariableClass> V =
            drv->Variables.GetVariableReferenceForContext(v.first, 0);
        if (V == nullptr) {
            Errors.push_back(std::string("Variable '") + v.first + "' not found in generated context");
        } else {
            if (V->Type() == v.second.Type()) {
                if (V->GetValue() == v.second.GetValue()) {


                } else {
                    std::ostringstream os;
                    os << "Expected: " << v.second.GetValue() << ", got: " << V->GetValue();
                    Errors.push_back(std::string("Variable '") + v.first + "' has wrong content, " + os.str());
                }
            } else {
                std::ostringstream os;
                os << "Expected: " << v.second.Type() << ", got: " << V->Type();
                Errors.push_back(std::string("Variable '") + v.first + "' has wrong type, " + os.str());
            }
        }
    }
    return Errors;
}

void MainWindow::TreeToSVG(std::list<std::shared_ptr<StatementClass>> Graph, std::string DotFilePath, std::string SVGFilePath)
{

    //if (Graph.empty()) {
    RestartNodeNumber();
    std::ofstream Drawing(DotFilePath);
    Drawing << "digraph g {" << std::endl;
    Drawing << "node [shape = record,height=.1];" << std::endl;
    Drawing << "compound=true" << std::endl;
    int CurrentNodeNumber = GetNextNodeNumber();
    Drawing << "Node" << CurrentNodeNumber << "[label = \"<f0> |<f1> Start |<f2> \"];" << std::endl;
    int GraphNumber = 0;
    int CurrentGraphNumber = GraphNumber++;
    for (auto &s: Graph) {
        if (s!=nullptr)  {
            int  NextNodeNumber = GetNextNodeNumber();
            int  NextGraphNumber = GraphNumber++;
            Drawing << "\"Node" << CurrentNodeNumber << "\":f0 -> \"Node" << NextNodeNumber << "\":f1" <<  "[ltail=cluster_g" << CurrentGraphNumber << " lhead=cluster_g" << NextGraphNumber << "];" << std::endl;
            //Drawing << "\"cluster_g" << CurrentGraphNumber << "\" -> \"cluster_g" << NextGraphNumber << "\";" << std::endl;
            Drawing << "subgraph cluster_g" << NextGraphNumber <<  "{" << std::endl;
            Drawing << "rank = same;" << std::endl;
            s->DrawNode(Drawing, NextNodeNumber);
            Drawing << "}" << std::endl;
            CurrentNodeNumber = NextNodeNumber;
            CurrentGraphNumber = NextGraphNumber;
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

bool MainWindow::CheckForTestTimeout()
{
    QApplication::processEvents();
    return TestTimeoutOccoured;
  }

void MainWindow::ExecutionStartTest(std::chrono::milliseconds TimeoutTime)
{
    TestTimeoutOccoured = false;
    TestTimer->start(TimeoutTime);
}

void MainWindow::ExecutionStoppTest()
{
    TestTimeoutOccoured = false;
    TestTimer->stop();
}

void MainWindow::TestTimeRunOut()
{
    TestTimeoutOccoured = true;
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

void MainWindow::setupFileMenu()
{
    QMenu *fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);

    fileMenu->addAction(tr("&New"), QKeySequence::New,
                        this, &MainWindow::newFile);
    fileMenu->addAction(tr("&Open..."), QKeySequence::Open,
                        this, [this](){ openFile(); });
    fileMenu->addAction(tr("&Save..."), QKeySequence::Save,
                        this, [this](){ saveFile(); });
    fileMenu->addAction(tr("E&xit"), QKeySequence::Quit,
                        qApp, &QApplication::quit);
}

void MainWindow::setupHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(helpMenu);

    helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About Syntax Highlighter"),
                       tr("<p>The <b>Syntax Highlighter</b> example shows how " \
                          "to perform simple syntax highlighting by subclassing " \
                          "the QSyntaxHighlighter class and describing " \
                          "highlighting rules using regular expressions.</p>"));
}

void MainWindow::newFile()
{
    editor->clear();
}

void MainWindow::openFile(QString path)
{
    QString fileName = path;

    if (fileName.isEmpty())
        fileName = QFileDialog::getOpenFileName(this, tr("Open File"), Documentpath, "OsiComp Files (*.occ)");

    if (!fileName.isEmpty()) {
        Documentpath = fileName;
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text))
            editor->setPlainText(file.readAll());
    }
}

void MainWindow::saveFile( QString path)
{
    QString fileName = path;

    if (fileName.isNull())
        fileName = QFileDialog::getSaveFileName(this, tr("Open File"), Documentpath, "OsiComp Files (*.occ)");

    if (!fileName.isEmpty()) {
        Documentpath = fileName;
        QFile file(fileName);
        if (file.open(QFile::WriteOnly | QFile::Text))
            file.write(editor->toPlainText().toLatin1());
    }

}

void MainWindow::ShowInputDialog(InputDialogClass::DialogDescriptor *Descriptor, std::vector<InputDialogClass::ValueType> *Result)
{
    //Stoprequest = true;
    InputDialogClass dialog(Descriptor, Result, this);

    if (dialog.exec() == QDialog::Accepted) {
        //  createLetter(dialog.senderName(), dialog.senderAddress(),
        //               dialog.orderItems(), dialog.sendOffers());
    } else {
        Result->clear();
    }

}



