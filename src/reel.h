#ifndef Reel_h
#define Reel_h

#include "config.h"
#include <Arduino.h>
#include <AccelStepper.h>


#define NUM_STEPS 48
#define STEPS_PER_VALUE 2
#define CALIBRATION_CONSTANT 2

class Reel {
    public:
        Reel(int reelIndex, AccelStepper &stepper, int sensorPin, int *reelSymbols, int reelSymbolsLength); // poslat tam jeste classu na ovladani svetel
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