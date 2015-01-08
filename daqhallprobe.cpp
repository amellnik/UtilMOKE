#include "daqhallprobe.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <math.h>
#include <windows.h>
#include "NIDAQmx.h"

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

DaqHallProbe::DaqHallProbe()
{

}

void DaqHallProbe::setup(std::string outChan, std::string readChan)
{
    output_chan_string = outChan;
    output_chan_char = output_chan_string.c_str();
    read_chan_string = readChan;
    read_chan_char = read_chan_string.c_str();

}

void DaqHallProbe::bias()
{
    DAQmxCreateTask("",&outputTaskHandle);
    DAQmxCreateAOVoltageChan (outputTaskHandle, output_chan_char, "", -10.0, 10.0, DAQmx_Val_Volts , NULL);
    DAQmxStartTask(outputTaskHandle);

    //Bias voltage for probe coded in here to avoid accidental changes that would invalidate calibration
    double bias_volts = 5.00;
    DAQmxWriteAnalogF64(outputTaskHandle,1,1,10.0,DAQmx_Val_GroupByChannel,&bias_volts,NULL,NULL);
    if( outputTaskHandle!=0 )
    {
        DAQmxStopTask(outputTaskHandle);
        DAQmxClearTask(outputTaskHandle);
    }
}

void DaqHallProbe::read(double &volts, double &tesla)
{   // Currently hard coded to read at 1kHz
     volts=0.0;
     int num_samps = 50;
     double results[50];
     //results=malloc(num_samps*sizeof(double));
     int temp;
     temp = DAQmxCreateTask("",&readTaskHandle);
     //int32 DAQmxCreateAIVoltageChan (TaskHandle taskHandle, const char physicalChannel[], const char nameToAssignToChannel[], int32 terminalConfig, float64 minVal, float64 maxVal, int32 units, const char customScaleName[]);
     temp = DAQmxCreateAIVoltageChan (readTaskHandle, read_chan_char, "", DAQmx_Val_Diff, -1, 1, DAQmx_Val_Volts , NULL);
     //DAQmxCfgSampClkTiming(taskHandle,"",sampleFreq,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,samplesPerChan);
     temp = DAQmxCfgSampClkTiming(readTaskHandle, "", 1000,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,num_samps);
     temp = DAQmxStartTask(readTaskHandle);
     //int32 DAQmxReadAnalogF64 (TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, bool32 fillMode, float64 readArray[], uInt32 arraySizeInSamps, int32 *sampsPerChanRead, bool32 *reserved);
     temp = DAQmxReadAnalogF64(readTaskHandle,num_samps,10,DAQmx_Val_GroupByChannel,results,num_samps,NULL,NULL);
     for (int i=0;i<num_samps;i++)
     {
         volts+=results[i];
     }
     volts /= num_samps;
     if( readTaskHandle!=0 )
     {
         DAQmxStopTask(readTaskHandle);
         DAQmxClearTask(readTaskHandle);
     }
     tesla = -.1826+1.2929*volts;
}


