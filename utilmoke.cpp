#include "utilmoke.h"
#include "ui_utilmoke.h"


UtilMOKE::UtilMOKE(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UtilMOKE)
{
    ui->setupUi(this);

}

UtilMOKE::~UtilMOKE()
{
    delete ui;
}
