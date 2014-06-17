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
    connect(ui->magStartBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->magEndBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->magDeltaBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->magDelayBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    }
    settingWidgetChanged(); //Load default values from UI into classes
    ui->bigGraph->addGraph();
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

void UtilMOKE::on_magSweep_clicked()
{
    double field=bigMag.start;
    data.clear_data();
    bigMag.ramp(field);
    TakeSingle();
    while (fabs(field-bigMag.end)>bigMag.delta)
    {
        field+=bigMag.delta*sgn(bigMag.end-field);
        bigMag.ramp(field);
        TakeSingle();
    }
    field=bigMag.end;
    bigMag.ramp(field);
    TakeSingle();
    while (fabs(field-bigMag.start)>bigMag.delta)
    {
        field+=bigMag.delta*sgn(bigMag.start-field);
        bigMag.ramp(field);
        TakeSingle();
    }
    field=bigMag.start;
    bigMag.ramp(field);
    TakeSingle();
    bigMag.ramp(0.0);

}

void UtilMOKE::TakeSingle()
{
    data.tesla.append(bigMag.now);
    data.volts.append(lockin.get_x());
    data.collected+=1;
    //ui->bigGraph->addGraph();
    ui->bigGraph->graph(0)->setData(data.tesla,data.volts);
    ui->bigGraph->xAxis->setLabel("Field (T)");
    ui->bigGraph->yAxis->setLabel("Signal from lockin (V)");
    ui->bigGraph->xAxis->setRange(bigMag.start,bigMag.end);
    ui->bigGraph->yAxis->setRange(data.min_volts(),data.max_volts());
    ui->bigGraph->replot();
}
