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
    ui->graphModeBox->addItem("Mag sweep"); // Index 0?
    ui->graphModeBox->addItem("Scan image"); // Index 1?
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
    on_graphModeBox_currentIndexChanged(0);

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

    for (mY=mirror.start_y;(fabs(mirror.end_y-mY)>mirror.delta_y)&&(interrupt==0);mY+=mirror.delta_y*sgn(mirror.end_y-mirror.start_y))
    {
        Sleep(mirror.delay*10); //Long initial delay to reset
        for (mX=mirror.start_x;(fabs(mX-mirror.end_x)>mirror.delta_x)&&(interrupt==0);mX+=mirror.delta_x*sgn(mirror.end_x-mirror.start_x))
        {

            mirror.set_dc(mX,mY);
            Sleep(mirror.delay);
            TakeSingle();
            QCoreApplication::processEvents();
        }
        mirror.set_dc(mirror.end_x,mY);
        TakeSingle();
    }
    mirror.set_dc(mirror.end_x,mirror.end_y);
    TakeSingle();
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
        QCoreApplication::processEvents();
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
    //    ui->bigGraph->graph(0)->setData(data.tesla,data.volts);
    //    ui->bigGraph->xAxis->setRange(bigMag.start,bigMag.end);
    //    ui->bigGraph->yAxis->setRange(data.min_volts(),data.max_volts());
    //
    int index=ui->graphModeBox->currentIndex();
    switch (index)
    {
    case 0: {
        twoDTrace->setData(data.tesla,data.volts);
        ui->bigGraph->xAxis->setRange(bigMag.start,bigMag.end);
        ui->bigGraph->yAxis->setRange(data.min_volts(),data.max_volts());
        ui->bigGraph->replot();
    } break;
    case 1: {
        int i;
        for (i=0;i<data.collected;i++){
            imageMap->data()->setData(data.mirrorX[i],data.mirrorY[i],data.volts[i]);
        }
        imageMap->setGradient(QCPColorGradient::gpCold);
        imageMap->rescaleDataRange(true);
        ui->bigGraph->xAxis->setRange(mirror.start_x,mirror.end_x);
        ui->bigGraph->yAxis->setRange(mirror.start_y,mirror.end_y);
        ui->bigGraph->rescaleAxes();
        ui->bigGraph->replot();
    }
    }
}

void UtilMOKE::on_interruptBox_clicked()
{
    interrupt=ui->interruptBox->isChecked();
}



void UtilMOKE::on_graphModeBox_currentIndexChanged(int index)
{
    //Make sure we have the right index by pulling it off the object itself so we can call this ourselves with impunity
    index=ui->graphModeBox->currentIndex();
    switch (index)
    {
    case 0: {
        ui->bigGraph->clearPlottables();
        ui->bigGraph->xAxis->setLabel("Field (T)");
        ui->bigGraph->yAxis->setLabel("Signal from lockin (V)");
        twoDTrace = new QCPCurve(ui->bigGraph->xAxis, ui->bigGraph->yAxis);
        ui->bigGraph->addPlottable(twoDTrace);
        ui->bigGraph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        twoDTrace->setPen(QPen(Qt::blue));
    } break;
    case 1: {
        ui->bigGraph->clearPlottables();
        ui->bigGraph->xAxis->setLabel("Mirror X (V)");
        ui->bigGraph->yAxis->setLabel("Mirror Y (V)");
        ui->bigGraph->axisRect()->setupFullAxesBox(true);
        imageMap = new QCPColorMap(ui->bigGraph->xAxis, ui->bigGraph->yAxis);
        ui->bigGraph->addPlottable(imageMap);
        int nx = (int) (mirror.end_x-mirror.start_x)/mirror.delta_x+1;
        int ny = (int) (mirror.end_y-mirror.start_y)/mirror.delta_y+1;
        imageMap->data()->setSize(nx,ny);
        imageMap->data()->setRange(QCPRange(mirror.start_x,mirror.end_x),QCPRange(mirror.start_y,mirror.end_y));
    } break;
    }
}
