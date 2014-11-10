#ifndef APTANGLE_H
#define APTANGLE_H
#include "APT/APT.h"

class APTAngle
{
public:
    double now;
    double start;
    double end;
    double angle;
    double delta;

    APTAngle();
    void set(double angle);
    void init();
    APT::MG17Motor motor;
};

#endif // APTANGLE_H
