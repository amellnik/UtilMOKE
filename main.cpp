#include "utilmoke.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UtilMOKE w;
    w.show();

    return a.exec();
}
