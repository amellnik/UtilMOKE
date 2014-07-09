#include "utilmoke.h"
#include "ui_utilmoke.h"
#include "lockin7265.h"
#include "daqmagcontrol.h"
#include "mokedata.h"
#include "pximirroraxes.h"
#include "lockin7270.h"


template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

UtilMOKE::UtilMOKE(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UtilMOKE)
{
    ui->setupUi(this);

    //Add values to combo boxes
    {
    ui->graphModeBox->addItem("Mag sweep"); // Index 0
    ui->graphModeBox->addItem("Scan image"); // Index 1
    ui->whatToPlotBox->addItem("Lockin Volts");// Index 0
    ui->whatToPlotBox->addItem("Lockin 2f Volts");// Index 1
    ui->whatToPlotBox->addItem("DC Volts"); // Index 2
    ui->whatToPlotBox->addItem("Lockin Volts / DC");// Index 3
    ui->whatToPlotBox->addItem("Lockin 2f Volts / DC");// Index 4
    }

    //What lockin model are we using?
    lockin_model = 7270;

    //Device channels and addresses
    mirror.set_chans("PXI1Slot2/ao0","PXI1Slot2/ao1" );
    bigMag.set_chan("Dev2/ao0");
    keithley.set_address(26);
    switch (lockin_model) {
        case 7265:lockin.set_address(27); break;
        case 7270:big_lockin.start("192.168.2.130",50000); break;
    }




    out_filename="";

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

    //Call this in case the mirror settings were changed and we need to adjust the image plot
    on_graphModeBox_currentIndexChanged(-1);
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
    //ClearData(); // Clear existing data if any
    data.clear_data();

    double mX=mirror.start_x;
    double mY=mirror.start_y;
    mirror.prep_sweep();

//    mirror.sweep_set(mX,mY);  // These three lines are a crappy bug fix that prevents the image plot from
//    TakeSingle();             // showing up unless you switch into the normal plotting mode and then back
//    on_graphModeBox_currentIndexChanged(1);

    for (mY=mirror.start_y;(fabs(mirror.end_y-mY)>=mirror.delta_y)&&(interrupt==0);mY+=mirror.delta_y*sgn(mirror.end_y-mirror.start_y))
    {
        Sleep(mirror.delay*10); //Long initial delay to reset
        for (mX=mirror.start_x;(fabs(mX-mirror.end_x)>=mirror.delta_x)&&(interrupt==0);mX+=mirror.delta_x*sgn(mirror.end_x-mirror.start_x))
        {

            mirror.sweep_set(mX,mY);
            Sleep(mirror.delay);
            TakeSingle();
            QCoreApplication::processEvents();
        }
        mirror.sweep_set(mirror.end_x,mY);
        TakeSingle();
    }
    mirror.sweep_set(mirror.end_x,mirror.end_y);
    TakeSingle();
    mirror.set_dc(0.0,0.0);
}

void UtilMOKE::on_magSweep_clicked()
{
   // ClearData(); //Clear existing data if any
    data.clear_data();

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
    switch (lockin_model) {
    case 7265: {
        data.lockin_volts.append(lockin.get_x());
        data.lockin_2f_volts.append(0.0); //Not used in this mode!
    } break;
    case 7270: {
        data.lockin_volts.append(big_lockin.get_x1());
        data.lockin_2f_volts.append(big_lockin.get_x2()); //Is used in this mode!
    } break;
    }
    data.tesla.append(bigMag.now);
    data.dc_volts.append(keithley.read());
    data.mirrorX.append(mirror.now_x);
    data.mirrorY.append(mirror.now_y);
    data.collected+=1;
    UpdateGraph();
}

void UtilMOKE::UpdateGraph()
{

    int whatToPlot=ui->whatToPlotBox->currentIndex();
    QVector<double> plottable;
    int i;
    switch (whatToPlot)
    {
    case 0: plottable=data.lockin_volts;  break; //Normal lockin volts
    case 1: plottable=data.lockin_2f_volts; break;  // 2f Lockin volts
    case 2: plottable=data.dc_volts; break; // DC volts
    case 3: {                                   // 1f / DC
        for (i=0;i<data.collected;i++){
            plottable.append(data.lockin_volts[i]/data.dc_volts[i]);
        }
    } break;
    case 4: {                                  //2f / DC
        for (i=0;i<data.collected;i++){
             plottable.append(data.lockin_2f_volts[i]/data.dc_volts[i]);
        }
    } break;
    }

    int index=ui->graphModeBox->currentIndex();
    switch (index)
    {
    case 0: { //Mag sweep mode
        twoDTrace->setData(data.tesla,plottable);
        ui->bigGraph->yAxis->rescale();
        ui->bigGraph->xAxis->setRange(bigMag.start,bigMag.end);
        ui->bigGraph->replot();
    } break;
    case 1: {//Image mode

        for (i=0;i<data.collected;i++){
            imageMap->data()->setData(data.mirrorX[i],data.mirrorY[i],plottable[i]);
        }
        ui->bigGraph->rescaleAxes();
        imageMap->rescaleDataRange();
        ui->bigGraph->replot();
    }
    }
}

void UtilMOKE::on_interruptBox_clicked()
{
    interrupt=ui->interruptBox->isChecked();
}

void UtilMOKE::ClearData(){ // Not currently used
    data.clear_data();
   switch (ui->graphModeBox->currentIndex())
   { // It's possible the later cases may not be initialized yet, clearing them would give a seg fault
   case 0: {
       twoDTrace->data()->clear();

   } break;
   case 1: {
        imageMap->data()->clear();
   } break;
   }
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

        //ui->bigGraph->plotLayout()->remove(colorScale);
        ui->bigGraph->addPlottable(twoDTrace);
        ui->bigGraph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        twoDTrace->setPen(QPen(Qt::blue));
    } break;
    case 1: {
        ui->bigGraph->clearPlottables();
        ui->bigGraph->xAxis->setLabel("Mirror X (V)");
        ui->bigGraph->yAxis->setLabel("Mirror Y (V)");
        imageMap = new QCPColorMap(ui->bigGraph->xAxis, ui->bigGraph->yAxis);
        ui->bigGraph->addPlottable(imageMap);
        //ui->bigGraph->xAxis->setRange(mirror.start_x,mirror.end_x);
        //ui->bigGraph->yAxis->setRange(mirror.start_y,mirror.end_y);
        ui->bigGraph->axisRect()->setupFullAxesBox(true);

        imageMap->setGradient(QCPColorGradient::gpHot);
        imageMap->rescaleDataRange(true);
        int nx = (int) floor((mirror.end_x-mirror.start_x)/mirror.delta_x+1);
        int ny = (int) floor((mirror.end_y-mirror.start_y)/mirror.delta_y+1);
        imageMap->data()->setSize(nx,ny);
        imageMap->data()->setRange(QCPRange(mirror.start_x,mirror.end_x),QCPRange(mirror.start_y,mirror.end_y));

         //Color scale stuff
//        colorScale = new QCPColorScale(ui->bigGraph);
//        ui->bigGraph->plotLayout()->addElement(0,1,colorScale);
//        colorScale->setType(QCPAxis::atRight);
//        imageMap->setColorScale(colorScale);
//        colorScale->axis()->setLabel("Lockin volts (V)");
//        marginGroup = new QCPMarginGroup(ui->bigGraph);
//        ui->bigGraph->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop,marginGroup);
//        colorScale->setMarginGroup(QCP::msBottom|QCP::msTop,marginGroup);

    } break;
    }
    //UpdateGraph(); //Why is this here again?
}

void UtilMOKE::on_saveDataButton_clicked()
{

    QString filename = QFileDialog::getSaveFileName(this,"Save file","default.dat",".dat");
    WriteFile(filename);
}

void UtilMOKE::WriteFile(QString filename) {
    out_file.setFileName(filename);
    if (out_file.open(QFile::WriteOnly|QFile::Truncate))
    {   int i;
        QTextStream output(&out_file);
        output << "Field" << "\t" << "Lockin" << "\t" << "Lockin_2f" <<
                  "\t" << "DC_volts" << "\t" << "MirrorX" << "\t" << "MirrorY" << "\n";
        for (i=0;i<data.collected;i++){
            output << data.tesla[i] << "\t" << data.lockin_volts[i] << "\t" << data.lockin_2f_volts[i] <<
                      "\t" << data.dc_volts[i] << "\t" << data.mirrorX[i] << "\t" << data.mirrorY[i] << "\n";
        }
        out_file.close();
    }
}

void UtilMOKE::on_test7270Button_clicked()
{
    big_lockin.test();
}
