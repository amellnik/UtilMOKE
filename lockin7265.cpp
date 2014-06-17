#include "lockin7265.h"
#include "windows.h"
#include "ni488.h"



Lockin7265::Lockin7265()
{

}

void Lockin7265::set_address(int ady) {
    address = ady;
    handle = ibdev(0, address, NO_SAD, T10s, 1, 0);
}

double Lockin7265::get_x()
{
    //Get value via gpib
    char buff[100];
    char *trash;
    //ibwrt (handle, "X.", strlen("X."));
    ibwrt (handle, const_cast<char*>("X."), strlen("X."));
    ibrd (handle, &buff, 100);
    return strtod(buff, &trash);
}
