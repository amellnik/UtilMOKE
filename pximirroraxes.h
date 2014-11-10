#ifndef PXIMIRRORAXES_H
#define PXIMIRRORAXES_H

#include <stdio.h>
#include <string>
#include <vector>
#include "NIDAQmx.h"

class PxiMirrorAxes
{
public:
    double now_x;
    double now_y;
    double start_x;
    double start_y;
    double end_x;
    double end_y;
    double delta_x;
    double delta_y;

    double line_x_start;
    double line_x_end;
    double line_y_start;
    double line_y_end;
    double line_step;

    int delay;
    double sampling_rate;
    int buffer_size;
    double buffer [16]; // Kinda arbitrary
    std::string chan_string_x;
    std::string chan_string_y;
    TaskHandle taskHandle;
    const char * chan_char_x;
    const char * chan_char_y;

    PxiMirrorAxes();
    void set_chans(std::string chanX, std::string chanY);
    void set_dc(double valX, double valY);
    void prep_sweep();
    void sweep_set(double valX, double valY);
};

#endif // PXIMIRRORAXES_H
