#include "utilmoke.h"
#include "daqmirroraxis.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UtilMOKE w;
    w.show();

    // Setup stuff


    // End setup
    return a.exec();
}
