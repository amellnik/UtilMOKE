#ifndef UTILMOKE_H
#define UTILMOKE_H

//#include "daqmirroraxis.h"
#include "daqmagcontrol.h"
#include "pximirroraxes.h"
#include "lockin7265.h"
#include "mokedata.h"
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
    void TakeSingle();
    void UpdateGraph();
    bool interrupt;



private slots:
    void settingWidgetChanged();
    void on_mirrorGotoSetpoints_clicked();
    void on_magGotoSetpoint_clicked();
    void on_magVoltsSetBox_editingFinished();

    void on_magSweep_clicked();

    void on_takeImage_clicked();

//    void on_interruptBox_stateChanged(int arg1);

    void on_interruptBox_clicked();

private:
    Ui::UtilMOKE *ui;
    PxiMirrorAxes mirror;
    DaqMagControl bigMag;
    Lockin7265 lockin;
    MOKEData data;
};

#endif // UTILMOKE_H
