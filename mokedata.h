#ifndef MOKEDATA_H
#define MOKEDATA_H
#include <QVector>

class MOKEData
{
public:

    QVector<double> lockin_x_volts; // Single channel on the 7265
    QVector<double> lockin_x1_volts;// x1 on the 7270
    QVector<double> lockin_x2_volts;// x2 on the 7270
    QVector<double> dc_volts;
    QVector<double> tesla;
    QVector<double> mirrorX;
    QVector<double> mirrorY;
    QVector<double> index;
    int collected;

    void clear_data();
    MOKEData();

};

#endif // MOKEDATA_H
