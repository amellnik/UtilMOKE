#include "daqmagcontrol.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>
#include <windows.h>
#include "NIDAQmx.h"

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

DaqMagControl::DaqMagControl()\
{
    set=0.0;
    now=0.0;
    start=0.0;
    end=0.0;
    delta=0.001;
    delay=0.0;
}

void DaqMagControl::set_chan(std::string thisChan)
{
    chan_string = thisChan;
    chan_char = chan_string.c_str();
}


void DaqMagControl::ramp(double this_set)
{
    double thisvolts;
    set = this_set;
    while (fabs(set-now)>delta)
    {
        now+=delta*sgn(set-now);

        // Magnetic field calibration goes here
        //thisvolts=0.026142-35.013*now+1.9131*pow(now,2)+4.6271*pow(now,3)-18.01*pow(now,4);

        //Currently using the low-field calibration, sweeping positive
        thisvolts = .10408-35.326*now+-12.366*pow(now,2)+59.107*pow(now,3)-1246*pow(now,4);

        set_volts(thisvolts);
        Sleep(10); // When we are changing the field by large amounts can sweep faster
    }
    Sleep(delay);
    now=set;
    thisvolts=0.026142-35.013*now+1.9131*pow(now,2)+4.6271*pow(now,3)-18.01*pow(now,4);
    set_volts(thisvolts);
}

void DaqMagControl::set_volts(double this_volts)
{
    DAQmxCreateTask("",&taskHandle);
    DAQmxCreateAOVoltageChan (taskHandle, chan_char, "", -10.0, 10.0, DAQmx_Val_Volts , NULL);
    DAQmxStartTask(taskHandle);
    double volts=this_volts;
    DAQmxWriteAnalogF64(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&volts,NULL,NULL);
    if( taskHandle!=0 )
    {
        DAQmxStopTask(taskHandle);
        DAQmxClearTask(taskHandle);
    }
}
