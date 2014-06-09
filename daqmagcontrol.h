#ifndef DAQMAGCONTROL_H
#define DAQMAGCONTROL_H

#include <stdio.h>
#include <string>
#include <vector>
#include "NIDAQmx.h"

class DaqMagControl
{
public:
    double set;
    double now;
    double start;
    double end;
    double delta;
    double delay;

    std::string chan_string;
    TaskHandle taskHandle;
    const char * chan_char;
    DaqMagControl();
    void set_chan(std::string thisChan);
    void ramp(double this_set);
    void set_volts(double this_volts);
};

#endif // DAQMAGCONTROL_H
