#include <QtGui/QApplication>
#include "BypassMonitor.h"

#include "typedef.h"
#include "if4api.h"
#include "nzcserver.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BypassMonitor w;

    /** VARAPI */
    loadVariable();
    if4Init();

    w.show();

    return a.exec();
}
