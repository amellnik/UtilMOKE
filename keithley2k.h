#ifndef KEITHLEY2K_H
#define KEITHLEY2K_H

class Keithley2k
{
public:
    int address;
    int handle;
    void set_address(int ady);
    double read();
    Keithley2k();
};

#endif // KEITHLEY2K_H
