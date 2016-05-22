#ifndef GDBMIWRITER_H
#define GDBMIWRITER_H

class QProcess;
class VDWindow;
class QString;

class GDBMIWriter
{
private:
    int varCounter;
    int varPrevCounter;
public:
    GDBMIWriter(QProcess * process);
    ~GDBMIWriter();
    void writeRun();
    void writeContinue();
    void writeStepin();
    void writeStepover();
    void writeStepout();
    void writeListLocals();
    void writeInterrupt();
    void writeQuit();
    void writeVarCreate(int token, QString varName);
    void writeVarDeleteAll();
    void writeVarListChildren(int token, QString varObjectName);
private:
    QProcess * process;
};

#endif // GDBMIWRITER_H
