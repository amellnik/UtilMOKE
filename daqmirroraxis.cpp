#include "daqmirroraxis.h"
#include <stdio.h>
#include <string>
#include <vector>
#include "NIDAQmx.h"

DaqMirrorAxis::DaqMirrorAxis()
{
    now=0.0;
    start=0.0;
    end=0.0;
}

void DaqMirrorAxis::set_chan(std::string thisChan)
{
    chan_string = thisChan;
    chan_char = chan_string.c_str();
}


DaqMirrorAxis::DaqMirrorAxis(std::string thisChan)
{
    now=0.0;
    start=0.0;
    end=0.0;

    chan_string = thisChan;
    chan_char = chan_string.c_str();
}

void DaqMirrorAxis::set(double val)
{
    DAQmxCreateTask("",&taskHandle);
    DAQmxCreateAOVoltageChan (taskHandle, chan_char, "", -10.0, 10.0, DAQmx_Val_Volts , NULL);
    DAQmxStartTask(taskHandle);
    now=val;
    DAQmxWriteAnalogF64(taskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&now,NULL,NULL);
    if( taskHandle!=0 )
    {
        DAQmxStopTask(taskHandle);
        DAQmxClearTask(taskHandle);
    }
}

