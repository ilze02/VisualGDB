#ifndef VDWINDOW_H
#define VDWINDOW_H

#include <QWidget>


class QPushButton;
class QPlainTextEdit;
class QString;
class QProcess;
class QSplitter;
class QGraphicsScene;
class QGraphicsView;
class QToolBar;
class QAction;
class GDBMIParser;
class QGraphicsPixmapItem;


class VDWindow : public QWidget
{
    Q_OBJECT
public:
    explicit VDWindow(QWidget *parent = 0);
    void emitSignalImageParced();
    ~VDWindow();
private:
    QAction *action_break;
    QAction *action_run;
    QAction *action_stepin;
    QAction *action_stepover;
    QAction *action_stepout;
    QAction *action_listlocals;
    QAction *action_stop;
    QAction *action_quit;
    QToolBar *toolbar;
    QSplitter *splitter;
    QPlainTextEdit *m_debug_output;
    QGraphicsView *view;
    QGraphicsScene *scene;
    QGraphicsPixmapItem * item;

    QString program;
    QProcess *process;
    bool appExecuting;

    GDBMIParser * parser;

signals:
    //void counterReached();
    void signalImageParced();

public slots:
    void slotOutputRecieved();
    void slotButtonClickedBreakMain();
    void slotButtonClickedRun();
    void slotButtonClickedStepin();
    void slotButtonClickedStepover();
    void slotButtonClickedStepout();
    void slotButtonClickedListLocals();
    void slotButtonClickedStop();
    void slotButtonClickedQuit();
    void slotImageParced();
};

#endif // VDWINDOW_H
