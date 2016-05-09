#include "vdwindow.h"
#include <QPushButton>
#include <QPlainTextEdit>
#include <QApplication>
#include <QString>
#include <QProcess>
#include <QByteArray>
#include <QSplitter>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QToolBar>
#include <QIcon>
#include <QLayout>
#include <QGraphicsItem>

VDWindow::VDWindow(QWidget *parent) : QWidget(parent)
{
    // Set size of the window
    //setFixedSize(1000, 600);
    QHBoxLayout *boxLayout = new QHBoxLayout();

    setLayout(boxLayout);
    // Create and position the button

    //button_break_main = new QPushButton("Break", this);
    //button_break_main->setGeometry(10, 10, 50, 30);
    //button_run = new QPushButton("Run", this);
    //button_run->setGeometry(70, 10, 50, 30);
    //button_c = new QPushButton("Cont.", this);
    //button_c->setGeometry(130, 10, 50, 30);
    //button_quit = new QPushButton("Quit", this);
    //button_quit->setGeometry(190, 10, 50, 30);
    toolbar = new QToolBar("Toolbar", this);
    QIcon icon_break(":/icon-break");
    action_break = new QAction(icon_break, "Break", 0);
    toolbar->addAction(action_break);
    QIcon icon_run(":/icon-run");
    action_run = new QAction(icon_run, "Run", 0);
    toolbar->addAction(action_run);
    QIcon icon_continue(":/icon-continue");
    action_continue = new QAction(icon_continue, "Continue", 0);
    toolbar->addAction(action_continue);
    QIcon icon_quit(":/icon-quit");
    action_quit = new QAction(icon_quit, "Quit", 0);
    toolbar->addAction(action_quit);

    toolbar->addSeparator();
    boxLayout->setMenuBar(toolbar);

    splitter = new QSplitter(this);
    boxLayout->addWidget(splitter);

    m_debug_output = new QPlainTextEdit("Text", this);
    m_debug_output->setGeometry(10, 50, 500, 200);
    m_debug_output->appendPlainText("Startup text.");
    splitter->addWidget(m_debug_output);
    view = new QGraphicsView(this);
    splitter->addWidget(view);

    scene = new QGraphicsScene();
    view->setScene(scene);
    QBrush blueBrush(Qt::blue);
    QPen outlinePen(Qt::black);
    QGraphicsRectItem *rect = scene->addRect(10, 10, 100, 100, outlinePen, blueBrush);
    rect->setFlag(QGraphicsItem::ItemIsMovable);

    program = "gdb MD1test.exe --interpreter=mi2";
    GDBProcess = new QProcess();

    GDBProcess->setProcessChannelMode(QProcess::MergedChannels);
    GDBProcess->setWorkingDirectory("D:\\Documents\\bakalaurs");
    GDBProcess->start(program);

    connect(GDBProcess, SIGNAL (readyReadStandardOutput()), this, SLOT (slotOutputRecieved()));

    connect(action_break, SIGNAL (triggered()), this, SLOT (slotButtonClickedBreakMain()));
    connect(action_run, SIGNAL (triggered()), this, SLOT (slotButtonClickedRun()));
    connect(action_continue, SIGNAL (triggered()), this, SLOT (slotButtonClickedContinue()));
    connect(action_quit, SIGNAL (triggered()), this, SLOT (slotButtonClickedQuit()));
}

void VDWindow::slotOutputRecieved()
{
    QByteArray answerData = GDBProcess->readAllStandardOutput();
    QString answer = answerData.trimmed();
    m_debug_output->appendPlainText(answer);
}

void VDWindow::slotButtonClickedBreakMain()
{
    GDBProcess->write("-break-insert main\n");
}

void VDWindow::slotButtonClickedRun()
{
    GDBProcess->write("-exec-run\n");
}

void VDWindow::slotButtonClickedContinue()
{
    GDBProcess->write("-exec-continue\n");
}

void VDWindow::slotButtonClickedQuit()
{
    GDBProcess->write("q\n");
}

