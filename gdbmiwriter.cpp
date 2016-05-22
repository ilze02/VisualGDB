#include "gdbmiwriter.h"
#include <QProcess>
#include <vdwindow.h>

GDBMIWriter::GDBMIWriter(QProcess * pr)
{
    process = pr;
    varCounter = 0;
    varPrevCounter = 1;
}

GDBMIWriter::~GDBMIWriter()
{

}

void GDBMIWriter::writeContinue()
{
    process->write("-exec-continue\n");
}

void GDBMIWriter::writeRun()
{
    process->write("-exec-run\n");
}

void GDBMIWriter::writeStepin()
{
    process->write("-exec-step\n");
}

void GDBMIWriter::writeStepover()
{
    process->write("-exec-next\n");
}

void GDBMIWriter::writeStepout()
{
    process->write("-exec-finish\n");
}

void GDBMIWriter::writeListLocals()
{
    writeVarDeleteAll();
    process->write("-stack-list-locals 0\n");
}

void GDBMIWriter::writeInterrupt()
{
    process->write("-exec-interrupt\n");
}

void GDBMIWriter::writeQuit()
{
    process->write("q\n");
}

void GDBMIWriter::writeVarCreate(int token, QString name)
{
    varCounter++;
    QString string1 = QString("%1-var-create v%2 * %3\n").arg(token).arg(varCounter).arg(name);
    process->write(string1.toUtf8().data());
}

void GDBMIWriter::writeVarDeleteAll()
{
    int j = varPrevCounter;
    varPrevCounter = varCounter + 1;
    for (int i = j; i <= varCounter; i++)
    {
        QString string = QString("-var-delete v%1\n").arg(i);
        process->write(string.toUtf8().data());
    }
}

void GDBMIWriter::writeVarListChildren(int token, QString varObjectName)
{
    QString string = QString("%1-var-list-children 0 %2\n").arg(token).arg(varObjectName);
    process->write(string.toUtf8().data());
}
