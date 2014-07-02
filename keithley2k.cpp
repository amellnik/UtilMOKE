#include "keithley2k.h"
#include "windows.h"
#include "ni488.h"

Keithley2k::Keithley2k()
{
}

void Keithley2k::set_address(int ady) {
    address = ady;
    handle = ibdev(0, address, NO_SAD,T10s,1,0);
}

double Keithley2k::read() {
//    char buf[20];
//        ibwrt (keithley.handle, KEITHLEY_FETCH, strlen(KEITHLEY_FETCH));
//        ibrd (keithley.handle, &buf, 20);
//        return atof(buf);
    char buff[100];
    char *trash;
    ibwrt(handle,const_cast<char*>("FETC?"),strlen("FETC?"));
    ibrd(handle,&buff,100);
    return strtod(buff,&trash);
}
