#include "utilmoke.h"
#include "daqmirroraxis.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UtilMOKE w;
    w.show();

    // Setup stuff
    DaqMirrorAxis mirrorX("Dev1/ao0"), mirrorY("Dev1/ao1");

    // End setup
    return a.exec();
}
