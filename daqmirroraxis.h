#ifndef DAQMIRRORAXIS_H
#define DAQMIRRORAXIS_H

#include <stdio.h>
#include <string>
#include <vector>
#include "NIDAQmx.h"

class DaqMirrorAxis
{

public:
    double now;
    double start;
    double end;
    std::string chan_string;
    TaskHandle taskHandle;
    const char * chan_char;
    DaqMirrorAxis();
    DaqMirrorAxis(std::string thisChan);
    void set_chan(std::string thisChan);
    void set(double val);
};

#endif // DAQMIRRORAXIS_H
