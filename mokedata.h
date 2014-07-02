#ifndef MOKEDATA_H
#define MOKEDATA_H
#include <QVector>

class MOKEData
{
public:

    QVector<double> lockin_volts;
    QVector<double> lockin_2f_volts;
    QVector<double> dc_volts;
    QVector<double> tesla;
    QVector<double> mirrorX;
    QVector<double> mirrorY;
    int collected;

    void clear_data();
    MOKEData();

};

#endif // MOKEDATA_H
