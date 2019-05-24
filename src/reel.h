#ifndef Reel_h
#define Reel_h

#include "config.h"
#include <Arduino.h>
#include <AccelStepper.h>
#include "flasher.h"
#include <avdweb_VirtualDelay.h>


#define NUM_STEPS 48
#define STEPS_PER_VALUE 2
#define CALIBRATION_CONSTANT 2

#define LIGHT_EFFECT_ON true


/*
Reel of a slot machine. It contains motor, calibration sensor and three lights
*/
class Reel {
    public:
        Reel(int reelIndex, AccelStepper &stepper, int *reelSymbols, int reelSymbolsLength, int sensorPin, Flasher &bulb1, Flasher &bulb2, Flasher &bulb3);

        bool tick();
        void init();
        void spin(int targetValue);
        bool isRunning();
        int* getSymbolsAfterSpin(int targetValue);
        void setLights(int *bulbArray);

    private:
        void calibrateReel();
        int getSymbolsIndex(long currentPos);
        int* getReelWinSymbols(long currentIndex);
        void spinningLightEffect();

        VirtualDelay lightEffectDelay;
        AccelStepper stepper;
        int sensorPin;
        int bulbPin1, bulbPin2, bulbPin3;
        int lightEffectPosition = 0;
        int effectWaterfall[4][3] = { {1,0,0}, {0,1,0}, {0,0,1}, {0,0,0} };
        int reelIndex;
        int *reelSymbols;
        int reelSymbolsLength;
        Flasher bulb1, bulb2, bulb3;
        int lightsStatus[3] = {0, 0, 0}; // possible values: 0 - off, 1 - permanent on, 2 - flash
        int reelWinSymbols[3] = {0, 0, 0};

        int speed = 120;
        int acceleration = 300;
};

#endif