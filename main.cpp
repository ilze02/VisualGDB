#include <QApplication>
#include <QProcess>
#include "vdwindow.h"
#include <gvc.h>

int main(int argc, char **argv)
{
    QApplication app (argc, argv);

    VDWindow window;
    window.showMaximized();

    return app.exec();
}
