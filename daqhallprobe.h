#ifndef DAQHALLPROBE_H
#define DAQHALLPROBE_H

#include <stdio.h>
#include <string>
#include <vector>
#include "NIDAQmx.h"

class DaqHallProbe
{
public:
    std::string output_chan_string;
    TaskHandle outputTaskHandle;
    const char * output_chan_char;
    std::string read_chan_string;
    TaskHandle readTaskHandle;
    const char * read_chan_char;

    DaqHallProbe();
    void setup(std::string outChan, std::string readChan);
    void bias();
    double readVolts();

};

#endif // DAQHALLPROBE_H
