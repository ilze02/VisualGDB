#include <QApplication>
#include <QProcess>
#include "vdwindow.h"
#include "vdgraph.h"
//#include <gvc.h>




int main(int argc, char **argv)
{

    QApplication app (argc, argv);

    VDWindow window;
    window.showMaximized();

    //VDGraph graph;


    return app.exec();
}
