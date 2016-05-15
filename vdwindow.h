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


class VDWindow : public QWidget
{
    Q_OBJECT
public:
    explicit VDWindow(QWidget *parent = 0);
private:
    QAction *action_break;
    QAction *action_run;
    QAction *action_continue;
    QAction *action_quit;
    QToolBar *toolbar;
    QSplitter *splitter;
    QPlainTextEdit *m_debug_output;
    QGraphicsView *view;
    QGraphicsScene *scene;

    QString program;
    QProcess *process;

    GDBMIParser * parser;

signals:
    //void counterReached();

public slots:
    void slotOutputRecieved();
    void slotButtonClickedBreakMain();
    void slotButtonClickedRun();
    void slotButtonClickedContinue();
    void slotButtonClickedQuit();
};

#endif // VDWINDOW_H
