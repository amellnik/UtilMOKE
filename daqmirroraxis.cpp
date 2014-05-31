#include "daqmirroraxis.h"
#include <stdio.h>
#include <string>
#include <vector>
#include "NIDAQmx.h"

DaqMirrorAxis::DaqMirrorAxis(std::string thisChan)
{
    now=0.0;
    start=0.0;
    end=0.0;
    //taskHandle = 0;
    chan_string = thisChan;
    chan_char = chan_string.c_str();


}

void DaqMirrorAxis::set(double val)
{
    DAQmxCreateTask("",&taskHandle);
    DAQmxCreateAOVoltageChan (taskHandle, chan_char, "", -10.0, 10.0, DAQmx_Val_Volts , NULL);
    now=val;
}

