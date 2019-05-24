#include "vfdcontrol.h"

/*
Uses the external library and extends its usability for use with the slot machine
*/
Vfdcontrol::Vfdcontrol(Samsung_16LF01_VFD &vfdisplay) {
    this->vfdisplay = vfdisplay;
}

void Vfdcontrol::init() {
    // Init the display, 16 digits and 20/31 of brightness
    vfdisplay.begin(16, 20);
    vfdisplay.clear();
}

void Vfdcontrol::tick() {

}