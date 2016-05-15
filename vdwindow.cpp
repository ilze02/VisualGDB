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

VDWindow::VDWindow(QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *boxLayout = new QHBoxLayout();

    setLayout(boxLayout);

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

    program = "gdb digitalais.exe --interpreter=mi2";
    process = new QProcess();

    process->setProcessChannelMode(QProcess::MergedChannels);
    process->setWorkingDirectory("/home/ilze/Documents");
    process->start(program);

    parser = new GDBMIParser(m_debug_output);

    connect(process, SIGNAL (readyReadStandardOutput()), this, SLOT (slotOutputRecieved()));

    connect(action_break, SIGNAL (triggered()), this, SLOT (slotButtonClickedBreakMain()));
    connect(action_run, SIGNAL (triggered()), this, SLOT (slotButtonClickedRun()));
    connect(action_continue, SIGNAL (triggered()), this, SLOT (slotButtonClickedContinue()));
    connect(action_quit, SIGNAL (triggered()), this, SLOT (slotButtonClickedQuit()));

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
    }

    //m_debug_output->appendPlainText(answerString);
}

void testArrays(QByteArray pArray)
{
int nSize = pArray.size();
char *pData = new char(nSize);
memcpy(pData, pArray.data(), nSize);
}

void VDWindow::slotButtonClickedBreakMain()
{
    process->write("-break-insert main\n");
}

void VDWindow::slotButtonClickedRun()
{
    process->write("-exec-run\n");
}

void VDWindow::slotButtonClickedContinue()
{
    process->write("-stack-list-locals --simple-values\n");
}

void VDWindow::slotButtonClickedQuit()
{
    process->write("q\n");
}

