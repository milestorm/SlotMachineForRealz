#ifndef Vfdcontrol_h
#define Vfdcontrol_h

#include "config.h"
#include <Arduino.h>
#include <Samsung_16LF01_VFD.h>
#include <avdweb_VirtualDelay.h>


/**
 * Extends the functions the Vacuum Fluorescent Display
 */
class Vfdcontrol {
    public:
        Vfdcontrol(Samsung_16LF01_VFD &vfdisplay);
        Vfdcontrol() {};
        void init();
        void tick();

    private:
        Samsung_16LF01_VFD vfdisplay;
};

#endif