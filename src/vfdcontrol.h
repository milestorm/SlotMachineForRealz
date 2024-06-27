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
        void update();
        void clear();
        void printToLeft(const char* text);
        void printToCenter(const char* text);
        void printToRight(const char* text);
        void counterStart(int startValue, int endValue, int delayMillis, int counterPrintPosition);
        bool isCounting();
        void printNumberTo(int number, int position);

    private:
        Samsung_16LF01_VFD vfdisplay;
        int currentValue = 0;
        int targetValue = 0;
        int increment = 1;
        int delayTime = 0;
        unsigned long  previousMillis = 0;
        bool counting = false;
        int counterPrintPosition = 1; // counterPrintPosition: 0, 1, 2 - left, center, right
        int getCurrentValue();
};

#endif