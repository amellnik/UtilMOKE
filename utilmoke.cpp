#include "utilmoke.h"
#include "ui_utilmoke.h"
#include "daqmirroraxis.h"

UtilMOKE::UtilMOKE(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UtilMOKE)
{
    ui->setupUi(this);
    mirrorX.set_chan("Dev1/ao0");
    mirrorY.set_chan("Dev1/ao1");
}

UtilMOKE::~UtilMOKE()
{
    delete ui;
}

void UtilMOKE::on_mirrorGotoSetpoints_clicked()
{
    mirrorX.set(ui->mirrorXSetBox->value());
    mirrorY.set(ui->mirrorYSetBox->value());
}
