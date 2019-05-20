#ifndef Reel_h
#define Reel_h

#include "config.h"
#include <Arduino.h>
#include <AccelStepper.h>
#include "flasher.h"


#define NUM_STEPS 48
#define STEPS_PER_VALUE 2
#define CALIBRATION_CONSTANT 2


/*
Reel of a slot machine. It contains motor, calibration sensor and three lights
*/
class Reel {
    public:
        Reel(int reelIndex, AccelStepper &stepper, int *reelSymbols, int reelSymbolsLength, int sensorPin); // poslat tam jeste classu na ovladani svetel

        bool tick();
        void init();
        void spin(int targetValue);
        bool isRunning();
        int* getSymbolsAfterSpin(int targetValue);
        int reelWinSymbols[3] = {0,0,0};

    private:
        AccelStepper stepper;
        void calibrateReel();
        int getSymbolsIndex(long currentPos);
        int* getReelWinSymbols(long currentIndex);

        int sensorPin;
        int reelIndex;
        int *reelSymbols;
        int reelSymbolsLength;

        int speed = 120;
        int acceleration = 300;
};

#endif