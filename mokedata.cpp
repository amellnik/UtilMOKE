#include "mokedata.h"
#include <QVector>

MOKEData::MOKEData()
{
    //Go ahead and allocate a lot of space for data
    volts.reserve(10000);
    tesla.reserve(10000);
    mirrorX.reserve(10000);
    mirrorY.reserve(10000);
    collected = 0;
}

void MOKEData::clear_data()
{
    volts.clear();
    tesla.clear();
    mirrorX.clear();
    mirrorY.clear();
    collected=0;
}

double MOKEData::max_volts()
{
    int i;
    double max=volts[0];
    for (i=0;i<collected;i++)
    {
        if (volts[i]>max) {
            max = volts[i];
        }
    }
    return max;
}

double MOKEData::min_volts()
{
    int i;
    double min=volts[0];
    for (i=0;i<collected;i++)
    {
        if (volts[i]<min) {
            min = volts[i];
        }
    }
    return min;
}
