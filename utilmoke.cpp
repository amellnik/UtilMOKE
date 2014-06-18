#include "utilmoke.h"
#include "ui_utilmoke.h"
#include "lockin7265.h"
#include "daqmagcontrol.h"
#include "mokedata.h"
#include "pximirroraxes.h"

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

UtilMOKE::UtilMOKE(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UtilMOKE)
{
    ui->setupUi(this);
    mirror.set_chans("PXI1Slot2/ao0","PXI1Slot2/ao1" );
    lockin.set_address(27);
    bigMag.set_chan("Dev2/ao0");
    bigMag.ramp(ui->magSetBox->value());
    //Connect a bunch of signals to slots
    {
    connect(ui->mirrorXStartBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->mirrorYStartBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->mirrorXEndBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->mirrorYEndBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->mirrorXDeltaBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->mirrorYDeltaBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->mirrorDelayBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));

    connect(ui->magStartBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->magEndBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->magDeltaBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->magDelayBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));

    }
    settingWidgetChanged(); //Load default values from UI into classes
    on_interruptBox_clicked(); // probably set the global interrupt to 0
    ui->bigGraph->addGraph();  //Set up some default stuff for the big graph
    ui->bigGraph->xAxis->setLabel("Field (T)");
    ui->bigGraph->yAxis->setLabel("Signal from lockin (V)");
}

UtilMOKE::~UtilMOKE()
{
    delete ui;
}

void UtilMOKE::settingWidgetChanged()
{
    //Update all values here because why not
    mirror.start_x=ui->mirrorXStartBox->value();
    mirror.start_y=ui->mirrorYStartBox->value();
    mirror.end_x=ui->mirrorXEndBox->value();
    mirror.end_y=ui->mirrorYEndBox->value();
    mirror.delta_x=ui->mirrorXDeltaBox->value();
    mirror.delta_y=ui->mirrorYDeltaBox->value();
    mirror.delay=ui->mirrorDelayBox->value();
    bigMag.start=ui->magStartBox->value();
    bigMag.end=ui->magEndBox->value();
    bigMag.delta=ui->magDeltaBox->value();
    bigMag.delay=ui->magDelayBox->value();

}

void UtilMOKE::on_mirrorGotoSetpoints_clicked()
{
    mirror.set_dc(ui->mirrorXSetBox->value(), ui->mirrorYSetBox->value());
}

void UtilMOKE::on_magGotoSetpoint_clicked()
{
    bigMag.ramp(ui->magSetBox->value());
}

void UtilMOKE::on_magVoltsSetBox_editingFinished()
{
    bigMag.set_volts(ui->magVoltsSetBox->value());
}

void UtilMOKE::on_takeImage_clicked()
{
    double mX=mirror.start_x;
    double mY=mirror.start_y;

    for (mY=mirror.start_y;(fabs(mY-mirror.end_y)<mirror.delta_y)&&(interrupt==0);mY+=mirror.delta_y*sgn(mirror.end_y-mirror.start_y))
    {
        Sleep(mirror.delay*10); //Long initial delay to reset
        for (mX=mirror.start_x;(fabs(mX-mirror.end_x)<mirror.delta_x)&&(interrupt==0);mX+=mirror.delta_x*sgn(mirror.end_x-mirror.start_x))
        {
            Sleep(mirror.delay);
            mirror.set_dc(mX,mY);
            TakeSingle();
            QCoreApplication::processEvents();
        }
    }
}

void UtilMOKE::on_magSweep_clicked()
{
    double field=bigMag.start;
    if (interrupt==0)
    {

        data.clear_data();
        bigMag.ramp(field);
        TakeSingle();
    }
    while ((fabs(field-bigMag.end)>bigMag.delta)&&(interrupt==0))
    {
        field+=bigMag.delta*sgn(bigMag.end-field);
        bigMag.ramp(field);
        TakeSingle();
        QCoreApplication::processEvents();
    }
    if (interrupt==0)
    {
        field=bigMag.end;
        bigMag.ramp(field);
        TakeSingle();
    }
    while ((fabs(field-bigMag.start)>bigMag.delta)&&(interrupt==0))
    {
        field+=bigMag.delta*sgn(bigMag.start-field);
        bigMag.ramp(field);
        TakeSingle();
    }
    if (interrupt==0){
        field=bigMag.start;
        bigMag.ramp(field);
        TakeSingle();
    }
    bigMag.ramp(0.0);
}

void UtilMOKE::TakeSingle()
{
    data.tesla.append(bigMag.now);
    data.volts.append(lockin.get_x());
    data.mirrorX.append(mirror.now_x);
    data.mirrorY.append(mirror.now_y);
    data.collected+=1;
    UpdateGraph();

}

void UtilMOKE::UpdateGraph()
{
    ui->bigGraph->graph(0)->setData(data.tesla,data.volts);
    ui->bigGraph->xAxis->setRange(bigMag.start,bigMag.end);
    ui->bigGraph->yAxis->setRange(data.min_volts(),data.max_volts());
    ui->bigGraph->replot();
}




void UtilMOKE::on_interruptBox_clicked()
{
    interrupt=ui->interruptBox->isChecked();
}

//void UtilMOKE::on_interruptBox_stateChanged(int arg1)
//{
//    //This is here because I made it at some point and don't know how to nuke it
//}
