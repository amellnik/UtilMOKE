#include "utilmoke.h"
#include "ui_utilmoke.h"
#include "lockin7265.h"
#include "daqmagcontrol.h"
#include "daqhallprobe.h"
#include "mokedata.h"
#include "pximirroraxes.h"
#include "lockin7270.h"
#include "aptangle.h"


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
    ui->graphModeBox->addItem("Line scan"); // Index 2


    ui->whatToPlotBox->addItem("7265 X Volts");// Index 0
    ui->whatToPlotBox->addItem("7270 X1 Volts");// Index 1
    ui->whatToPlotBox->addItem("7270 X2 Volts");// Index 2
    ui->whatToPlotBox->addItem("DC Volts"); // Index 3
    ui->whatToPlotBox->addItem("X / DC");// Index 4
    ui->whatToPlotBox->addItem("X1 / DC");// Index 5
    ui->whatToPlotBox->addItem("X2 / DC");// Index 6
    }

    //What lockin model are we using?  Options are 7265, 7270, both
    int both = 7265+7270; // = 14535
    lockin_model = both;

    //Device channels and addresses
    mirror.set_chans("PXI1Slot2/ao0","PXI1Slot2/ao1" );
    bigMag.set_chan("Dev2/ao0");
    bigMagProbe.setup("Dev2/ao1", "Dev2/ai0");
    keithley.set_address(26);
    switch (lockin_model) {
        case 7265:lockin.set_address(27); break;
        case 7270:big_lockin.start("192.168.1.119",50000); break;
        case 14535: lockin.set_address(27);big_lockin.start("192.168.1.119",50000); break;
    }
    out_filename="";

    bigMag.ramp(ui->magSetBox->value());

    //Turn on bias to Hall Probe
    bigMagProbe.bias();

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

    connect(ui->angleStartBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->angleEndBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->angleDeltaBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));

    connect(ui->lineXStartBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->lineXEndBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->lineYStartBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->lineYEndBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    connect(ui->lineStepBox, SIGNAL(editingFinished()),this,SLOT(settingWidgetChanged()));
    }
    settingWidgetChanged(); //Load default values from UI into classes
    on_interruptBox_clicked(); // probably set the global interrupt to 0

    ui->bigGraph->addGraph();  //Set up some default stuff for the big graph
    on_graphModeBox_currentIndexChanged(0);

    angle.init();

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
    angle.start=ui->angleStartBox->value();
    angle.end=ui->angleEndBox->value();
    angle.delta=ui->angleDeltaBox->value();
    mirror.line_x_start=ui->lineXStartBox->value();
    mirror.line_x_end=ui->lineXEndBox->value();
    mirror.line_y_start=ui->lineYStartBox->value();
    mirror.line_y_end=ui->lineYEndBox->value();
    mirror.line_step=ui->lineStepBox->value();

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
    double hallV; double hallT;
    Sleep(500); // Wait half a sec for things to stabilize before measuring
    bigMagProbe.read(hallV, hallT);
    ui->hallProbeVBox->setValue(hallV);
    ui->hallProbeTBox->setValue(hallT);
}

void UtilMOKE::on_takeImage_clicked()
{
    data.clear_data();

    double mX=mirror.start_x;
    double mY=mirror.start_y;
    mirror.prep_sweep();

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

        //data.clear_data();
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
        data.lockin_x_volts.append(lockin.get_x());
        data.lockin_x1_volts.append(0.0); //Not used in this mode!
        data.lockin_x2_volts.append(0.0); //Not used in this mode!
    } break;
    case 7270: {
        data.lockin_x_volts.append(0.0); //Not used in this mode!
        data.lockin_x1_volts.append(big_lockin.get_x1());
        data.lockin_x2_volts.append(big_lockin.get_x2());
    } break;
    case 14535: {
        data.lockin_x_volts.append(lockin.get_x());
        data.lockin_x1_volts.append(big_lockin.get_x1());
        data.lockin_x2_volts.append(big_lockin.get_x2());
    } break;
    }
    data.index.append(data.collected*mirror.line_step);
    data.tesla.append(bigMag.now);
    data.dc_volts.append(keithley.read());
    data.mirrorX.append(mirror.now_x);
    data.mirrorY.append(mirror.now_y);
    data.mirrorLine.append(sqrt(pow(mirror.now_x-mirror.line_x_start,2)+pow(mirror.now_y-mirror.line_y_start,2)));
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
//    ui->whatToPlotBox->addItem("7265 X Volts");// Index 0
//    ui->whatToPlotBox->addItem("7270 X1 Volts");// Index 1
//    ui->whatToPlotBox->addItem("7270 X2 Volts");// Index 2
//    ui->whatToPlotBox->addItem("DC Volts"); // Index 3
//    ui->whatToPlotBox->addItem("X / DC");// Index 4
//    ui->whatToPlotBox->addItem("X1 / DC");// Index 5
//    ui->whatToPlotBox->addItem("X2 / DC");// Index 6

    case 0: plottable=data.lockin_x_volts;  break; //Normal lockin volts
    case 1: plottable=data.lockin_x1_volts; break;  // 7270 X1
    case 2: plottable=data.lockin_x2_volts; break; // 7270 x2
    case 3: plottable =data.dc_volts; break; //DC
    case 4: {                                   // x / DC
        for (i=0;i<data.collected;i++){
            plottable.append(data.lockin_x_volts[i]/data.dc_volts[i]);
        }
    } break;
    case 5: {                                  //x1 / DC
        for (i=0;i<data.collected;i++){
             plottable.append(data.lockin_x1_volts[i]/data.dc_volts[i]);
        }
    } break;
    case 6: {                                  //x1 / DC
        for (i=0;i<data.collected;i++){
             plottable.append(data.lockin_x2_volts[i]/data.dc_volts[i]);
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
    } break;
    case 2: { //Line scan mode
        twoDTrace->setData(data.mirrorLine,plottable);
        ui->bigGraph->yAxis->rescale();
        if (data.collected>0){
            ui->bigGraph->xAxis->setRange(0,sqrt(pow(mirror.line_x_start-mirror.line_x_end,2)+pow(mirror.line_y_start-mirror.line_y_end,2)));
        }
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
   case 2:{
       twoDTrace->data()->clear();
   }
   }
}

void UtilMOKE::on_graphModeBox_currentIndexChanged(int index)
{
    //Make sure we have the right index by pulling it off the object itself so we can call this ourselves with impunity

    index=ui->graphModeBox->currentIndex();
    switch (index)
    {
    case 0: { // Mag sweep mode
        ui->bigGraph->clearPlottables();
        ui->bigGraph->xAxis->setLabel("Field (T)");
        ui->bigGraph->yAxis->setLabel("Selected voltage signal (V)");
        twoDTrace = new QCPCurve(ui->bigGraph->xAxis, ui->bigGraph->yAxis);
        ui->bigGraph->addPlottable(twoDTrace);
        ui->bigGraph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        twoDTrace->setPen(QPen(Qt::blue));
    } break;
    case 1: { // Image scan mode
        ui->bigGraph->clearPlottables();
        ui->bigGraph->xAxis->setLabel("Mirror X (V)");
        ui->bigGraph->yAxis->setLabel("Mirror Y (V)");
        imageMap = new QCPColorMap(ui->bigGraph->xAxis, ui->bigGraph->yAxis);
        ui->bigGraph->addPlottable(imageMap);
        ui->bigGraph->axisRect()->setupFullAxesBox(true);
        imageMap->setGradient(QCPColorGradient::gpHot);
        imageMap->rescaleDataRange(true);
        int nx = (int) floor((mirror.end_x-mirror.start_x)/mirror.delta_x+1);
        int ny = (int) floor((mirror.end_y-mirror.start_y)/mirror.delta_y+1);
        imageMap->data()->setSize(nx,ny);
        imageMap->data()->setRange(QCPRange(mirror.start_x,mirror.end_x),QCPRange(mirror.start_y,mirror.end_y));
    } break;
    case 2: { // Line scan mode
        ui->bigGraph->clearPlottables();
        ui->bigGraph->xAxis->setLabel("Position along line (V)");
        ui->bigGraph->yAxis->setLabel("Selected voltage signal (V)");
        twoDTrace = new QCPCurve(ui->bigGraph->xAxis, ui->bigGraph->yAxis);
        ui->bigGraph->addPlottable(twoDTrace);
        ui->bigGraph->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
        twoDTrace->setPen(QPen(Qt::blue));
    } break;
    }
    UpdateGraph(); //Why is this here again?
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
        output << "Field" << "\t" << "X" << "\t" << "X1" << "\t" << "X2"
                  "\t" << "DC_volts" << "\t" << "MirrorX" << "\t" << "MirrorY" << "\t" << "MirrorLine" << "\n";
        for (i=0;i<data.collected;i++){
            output << data.tesla[i] << "\t" << data.lockin_x_volts[i] << "\t" << data.lockin_x1_volts[i] <<
                      "\t" << data.lockin_x2_volts[i] << "\t" << data.dc_volts[i] << "\t" << data.mirrorX[i] <<
                      "\t" << data.mirrorY[i] << "\t" << data.mirrorLine[i] << "\n";
        }
        out_file.close();
    }
}

void UtilMOKE::on_test7270Button_clicked()
{
    big_lockin.test();
}

void UtilMOKE::on_whatToPlotBox_currentIndexChanged(int index)
{
    UpdateGraph();
}

void UtilMOKE::on_angleGotoButton_clicked()
{
    angle.set(ui->angleSetBox->value());
}

void UtilMOKE::on_autoSaveButton_clicked()
{
    QString start = ui->pathAndPrefixEdit->text();
    QString filename;
    QTextStream(&filename) << start << "_" << angle.now << "deg_" << ui->ref_rSpinBox->value() << "Ohm_" << bigMag.now <<  "T.dat";
    WriteFile(filename);
}

void UtilMOKE::on_takeLineScanButton_clicked()
{
    // Line scans append both field directions to the same file if the swapScan box is checked
    double thisMag = ui->magSetBox->value();
    bigMag.ramp(-.05);
    Sleep(500);
    bigMag.ramp(thisMag);
    Sleep(500);
    data.clear_data();
    double mX=mirror.line_x_start;
    double mY=mirror.line_y_start;
    double dX = (mirror.line_x_end - mirror.line_x_start); //Full distance not step size in that direction
    double dY = (mirror.line_y_end - mirror.line_y_start);
    double angle = qAtan2(dY,dX);
    mirror.prep_sweep();
    while ((sqrt(pow(mX-mirror.line_x_end,2)+pow(mY-mirror.line_y_end,2))>mirror.line_step)&&(interrupt==0))
    {
            mirror.sweep_set(mX,mY);
            Sleep(mirror.delay);
            TakeSingle();
            QCoreApplication::processEvents();
            mX += qCos(angle)*mirror.line_step;
            mY += qSin(angle)*mirror.line_step;
    }
    mirror.set_dc(0.0,0.0);
    bool swapScan = ui->swapScansBox->isChecked();
    if (swapScan&&(interrupt==0)) {
        //Swap magnetic field here
        thisMag = -ui->magSetBox->value();
        bigMag.ramp(-.05);
        Sleep(500);
        bigMag.ramp(thisMag);
        ui->magSetBox->setValue(thisMag);
        Sleep(500);

        mX=mirror.line_x_start;
        mY=mirror.line_y_start;
        mirror.prep_sweep();
        while ((sqrt(pow(mX-mirror.line_x_end,2)+pow(mY-mirror.line_y_end,2))>mirror.line_step)&&(interrupt==0))
        {
                mirror.sweep_set(mX,mY);
                Sleep(mirror.delay);
                TakeSingle();
                QCoreApplication::processEvents();
                mX += qCos(angle)*mirror.line_step;
                mY += qSin(angle)*mirror.line_step;
        }
        mirror.set_dc(0.0,0.0);
    }
}

void UtilMOKE::on_stepAngleSweepButton_clicked()
{
    angle.now=angle.start;
    int index;
    while ((fabs(angle.end-angle.now)>angle.delta)&&(interrupt==0)) {
        angle.set(angle.now);
        ui->angleSetBox->setValue(angle.now);
        index=ui->graphModeBox->currentIndex();
        switch (index)
        {
        case 0: {
            on_magSweep_clicked();
        } break;
        case 1: {
            on_takeImage_clicked();
        } break;
        case 2: {
            on_takeLineScanButton_clicked();
        } break;
        }
        on_autoSaveButton_clicked();
        angle.now+=angle.delta*sgn(angle.end-angle.start);
        ui->angleSetBox->setValue(angle.now);
    }

}

void UtilMOKE::on_mirrorTestButton_clicked()
{
    int step=0;
    while(interrupt==0){
        if ((step%2)==0){
            mirror.set_dc(0,0);
        } else {
            int index=(step%8);
            switch(index){
            case 1: mirror.set_dc(10,0); break;
            case 3: mirror.set_dc(0,10); break;
            case 5: mirror.set_dc(-10,0); break;
            case 7: mirror.set_dc(0,-10); break;
            }
        }
        Sleep(500);
        step++;
        QCoreApplication::processEvents();
    }
    mirror.set_dc(0,0);
}
