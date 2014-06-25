#include "pximirroraxes.h"
#include <stdio.h>
#include <string>
#include <vector>
#include "NIDAQmx.h"
#include "utilmoke.h"
#include "ui_utilmoke.h"

PxiMirrorAxes::PxiMirrorAxes()
{
    now_x=0.0;
    now_y=0.0;
    start_x=0.0;
    start_y=0.0;
    end_x=0.0;
    end_y=0.0;
    delay=0;
    delta_x=0.0;
    delta_y=0.0;
    sampling_rate = 32000.0; //Kinda arbitrary
    buffer_size = sizeof(buffer)/sizeof(buffer[0]);
}

void PxiMirrorAxes::set_chans(std::string chanX, std::string chanY)
{
    chan_string_x = chanX;
    chan_string_y = chanY;
    chan_char_x = chan_string_x.c_str();
    chan_char_y = chan_string_y.c_str();
}

void PxiMirrorAxes::set_dc(double valX, double valY)
{
    int i;
    for (i=0;i<buffer_size/2;i++)
    {
        buffer[i]=valX;
        buffer[i+buffer_size/2]=valY;
    }
    if (taskHandle) {  // If a task is currently running stop it
        DAQmxStopTask(taskHandle);
        DAQmxClearTask(taskHandle);
        taskHandle = 0;
    }
    int err;
    err= DAQmxCreateTask("",&taskHandle);
    err = DAQmxCreateAOVoltageChan (taskHandle, chan_char_x, "", -10.0, 10.0, DAQmx_Val_Volts , NULL);
    err = DAQmxCreateAOVoltageChan (taskHandle, chan_char_y, "", -10.0, 10.0, DAQmx_Val_Volts , NULL);
    err = DAQmxCfgSampClkTiming(taskHandle,"",sampling_rate,DAQmx_Val_Rising,DAQmx_Val_ContSamps,buffer_size/2);

    int32 written;
    err = DAQmxWriteAnalogF64(taskHandle,buffer_size/2,0,10.0,DAQmx_Val_GroupByChannel,buffer,&written,NULL);
    err = DAQmxStartTask(taskHandle);
    now_x = valX;
    now_y=valY;
}
