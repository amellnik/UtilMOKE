#include "mokedata.h"
#include <QVector>

MOKEData::MOKEData()
{
    //Go ahead and allocate a lot of space for data
    lockin_x_volts.reserve(10000);
    lockin_x1_volts.reserve(10000);
    lockin_x2_volts.reserve(10000);
    dc_volts.reserve(10000);
    tesla.reserve(10000);
    mirrorX.reserve(10000);
    mirrorY.reserve(10000);
    mirrorLine.reserve(10000);
    index.reserve(10000);
    collected = 0;
}

void MOKEData::clear_data()
{
    lockin_x_volts.clear();
    lockin_x1_volts.clear();
    lockin_x2_volts.clear();
    dc_volts.clear();
    tesla.clear();
    mirrorX.clear();
    mirrorY.clear();
    mirrorLine.clear();
    index.clear();
    collected=0;
}

