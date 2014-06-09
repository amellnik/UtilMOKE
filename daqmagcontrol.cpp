#include "daqmagcontrol.h"
#include <stdio.h>
#include <string>
#include <vector>
#include "NIDAQmx.h"

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
    set = this_set;
    //Do something here

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
