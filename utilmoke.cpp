#include "utilmoke.h"
#include "ui_utilmoke.h"
//#include "daqmirroraxis.h"
#include "daqmagcontrol.h"
#include "pximirroraxes.h"

UtilMOKE::UtilMOKE(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UtilMOKE)
{
    ui->setupUi(this);
    mirror.set_chans("PXI1Slot2/ao0","PXI1Slot2/ao1" );
    bigMag.set_chan("Dev1/ao0");
}

UtilMOKE::~UtilMOKE()
{
    delete ui;
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
