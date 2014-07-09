#ifndef UTILMOKE_H
#define UTILMOKE_H

//#include "daqmirroraxis.h"
#include "daqmagcontrol.h"
#include "pximirroraxes.h"
#include "lockin7265.h"
#include "mokedata.h"
#include "qcustomplot.h"
#include "keithley2k.h"
#include <QWidget>
#include "lockin7270.h"


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
    void ClearData();
    void WriteFile(QString filename);
    bool interrupt;



private slots:
    void settingWidgetChanged();
    void on_mirrorGotoSetpoints_clicked();
    void on_magGotoSetpoint_clicked();
    void on_magVoltsSetBox_editingFinished();
    void on_magSweep_clicked();
    void on_takeImage_clicked();
    void on_interruptBox_clicked();

    void on_graphModeBox_currentIndexChanged(int index);

    void on_saveDataButton_clicked();

    void on_test7270Button_clicked();

private:
    Ui::UtilMOKE *ui;
    PxiMirrorAxes mirror;
    DaqMagControl bigMag;
    Lockin7265 lockin;
    Lockin7270 big_lockin;
    Keithley2k keithley;
    int lockin_model;
    MOKEData data;

    QFile out_file;
    QString out_filename;

    QCPCurve *twoDTrace;
    QCPColorMap *imageMap;
    QCPColorScale *colorScale;
    QCPMarginGroup *marginGroup;

};

#endif // UTILMOKE_H
