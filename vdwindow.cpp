#include "vdwindow.h"
#include "gdbmiparser.h"
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
#include <QAction>
#include <QMessageBox>
#include <QPixmap>
#include <gdbmiwriter.h>
#include <vdvariablelist.h>

VDWindow::VDWindow(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *boxLayout = new QHBoxLayout();

    setLayout(boxLayout);

    toolbar = new QToolBar("Toolbar", this);
    QIcon icon_break(":/icon-break");
    action_break = new QAction(icon_break, "Break main", 0);
    toolbar->addAction(action_break);
    QIcon icon_run(":/icon-run");
    action_run = new QAction(icon_run, "Run", 0);
    toolbar->addAction(action_run);
    QIcon icon_stepin(":/icon-stepin");
    action_stepin = new QAction(icon_stepin, "Step in", 0);
    toolbar->addAction(action_stepin);
    QIcon icon_stepover(":/icon-stepover");
    action_stepover = new QAction(icon_stepover, "Step over", 0);
    toolbar->addAction(action_stepover);
    QIcon icon_stepout(":/icon-stepout");
    action_stepout = new QAction(icon_stepout, "Step out", 0);
    toolbar->addAction(action_stepout);
    QIcon icon_listlocals(":/icon-listlocals");
    action_listlocals = new QAction(icon_listlocals, "List locals", 0);
    toolbar->addAction(action_listlocals);
    QIcon icon_stop(":/icon-stop");
    action_stop = new QAction(icon_stop, "Interrupt", 0);
    toolbar->addAction(action_stop);
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
    QPixmap image;
    item = scene->addPixmap(image);
    view->show();

    //QBrush blueBrush(Qt::blue);
    //QPen outlinePen(Qt::black);
    //QGraphicsRectItem *rect = scene->addRect(10, 10, 100, 100, outlinePen, blueBrush);
    //rect->setFlag(QGraphicsItem::ItemIsMovable);

    program = "gdb testapp -iex \"set auto-load safe-path /\" --interpreter=mi2";
    process = new QProcess();

    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setWorkingDirectory("/home/ilze/Documents");
    process->start(program);
    appExecuting = false;

    parser = new GDBMIParser(m_debug_output, this);
    writer = new GDBMIWriter(process);
    varList = new VDVariableList(parser, writer, this);

    connect(process, SIGNAL (readyReadStandardOutput()), this, SLOT (slotOutputRecieved()));

    connect(action_break, SIGNAL (triggered()), this, SLOT (slotButtonClickedBreakMain()));
    connect(action_run, SIGNAL (triggered()), this, SLOT (slotButtonClickedRun()));
    connect(action_stepin, SIGNAL (triggered()), this, SLOT (slotButtonClickedStepin()));
    connect(action_stepover, SIGNAL (triggered()), this, SLOT (slotButtonClickedStepover()));
    connect(action_stepout, SIGNAL (triggered()), this, SLOT (slotButtonClickedStepout()));
    connect(action_listlocals, SIGNAL (triggered()), this, SLOT (slotButtonClickedListLocals()));
    connect(action_stop, SIGNAL (triggered()), this, SLOT (slotButtonClickedStop()));
    connect(action_quit, SIGNAL (triggered()), this, SLOT (slotButtonClickedQuit()));
    connect(this, SIGNAL(signalImageParced()), this, SLOT(slotImageParced()));
}

VDWindow::~VDWindow()
{
    delete toolbar;
    delete action_break;
    delete action_run;
    delete action_stepin;
    delete action_stepover;
    delete action_stepout;
    delete action_listlocals;
    delete action_stop;
    delete action_quit;
    delete splitter;
    process->write("q\n");
    process->close();
    delete process;
    delete parser;
    delete writer;
    delete varList;
}

void VDWindow::slotOutputRecieved()
{
    QByteArray answerData = process->readAllStandardOutput();
    //QString answer = answerData.trimmed();
    int size = answerData.size();
    char * answerString = new char[size + 1];
    memcpy(answerString, answerData.data(), size);
    answerString[size] = '\0';

    if (parser->doParse(answerString) == false)
    {
        //TODO - do something with the error
        QMessageBox msgBox;
        msgBox.setText("Parser got error.");
        msgBox.exec();
    }

    //m_debug_output->appendPlainText(answerString);
}

void testArrays(QByteArray pArray)
{
    int nSize = pArray.size();
    char *pData = new char(nSize);
    memcpy(pData, pArray.data(), nSize);
}


void VDWindow::emitSignalImageParced()
{
    emit signalImageParced();
}

void VDWindow::slotButtonClickedBreakMain()
{
    //process->write("-break-insert testapp.cpp:30\n");
    process->write("-break-insert main\n");
}

void VDWindow::slotButtonClickedRun()
{
    if (appExecuting)
    {
        writer->writeContinue();
    }
    else
    {
        appExecuting = true;
        writer->writeRun();
    }
}

void VDWindow::slotButtonClickedStepin()
{
    writer->writeStepin();
}

void VDWindow::slotButtonClickedStepover()
{
    writer->writeStepover();
}

void VDWindow::slotButtonClickedStepout()
{
    writer->writeStepout();
}

void VDWindow::slotButtonClickedListLocals()
{
    writer->writeVarDeleteAll();
    delete varList;
    varList = new VDVariableList(parser, writer, this);
    writer->writeListLocals();
}

void VDWindow::slotButtonClickedStop()
{
    writer->writeInterrupt();
}

void VDWindow::slotButtonClickedQuit()
{
    writer->writeQuit();
}

void VDWindow::slotImageParced()
{
    QPixmap image("../graphviztest.svg");
    item->setPixmap(image);
    view->show();
}

