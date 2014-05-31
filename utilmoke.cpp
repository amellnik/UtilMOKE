#include "utilmoke.h"
#include "ui_utilmoke.h"
#include "daqmirroraxis.h"

UtilMOKE::UtilMOKE(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UtilMOKE)
{
    ui->setupUi(this);
    DaqMirrorAxis mirrorX("Dev1/ai0"), mirrorY("Dev1/ai1");

}

UtilMOKE::~UtilMOKE()
{
    delete ui;
}
