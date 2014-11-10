#include "aptangle.h"
#include "APT/APT.h"

APTAngle::APTAngle()
{
    now = 0;
    start=0;
    end=0;
    delta=0;
}

void APTAngle::set(double angle)
{
    angle += 61.0; // offset for the half wave plate
    if (angle<360) {
        angle-=360;
    }
    motor.MoveAbsoluteRot(0,angle,angle,3,1);
}

void APTAngle::init()
{
    motor.SetHWSerialNum(83853589);
    motor.StartCtrl();
    motor.MoveHome(0,1);
    //motor.MoveAbsoluteRot(0,61,61,3,0);
}
