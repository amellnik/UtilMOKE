#ifndef MOKEDATA_H
#define MOKEDATA_H
#include <QVector>

class MOKEData
{
public:

    QVector<double> volts;
    QVector<double> tesla;
    int collected;

    void clear_data();
    double max_volts();
    double min_volts();
    MOKEData();

};

#endif // MOKEDATA_H
