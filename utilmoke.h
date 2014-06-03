#ifndef UTILMOKE_H
#define UTILMOKE_H

#include "daqmirroraxis.h"
#include <QWidget>

namespace Ui {
class UtilMOKE;
}

class UtilMOKE : public QWidget
{
    Q_OBJECT

public:
    explicit UtilMOKE(QWidget *parent = 0);
    ~UtilMOKE();

private slots:
    void on_mirrorGotoSetpoints_clicked();

private:
    Ui::UtilMOKE *ui;
    DaqMirrorAxis mirrorX;
    DaqMirrorAxis mirrorY;
};

#endif // UTILMOKE_H
