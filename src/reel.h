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

        void tick();
        void init();
        void spin(int targetValue);
        bool isRunning();
        uint16_t* getSymbolsAfterSpin(uint16_t targetValue);
        void setLights(int *bulbArray);
        uint16_t calculateTargetValue(int targetPos);
        int findSymbolIndex(int winningSymbol, uint16_t targetMotorValueReel);
        int getFutureSymbolsIndex(uint16_t targetValue);
        int calculateAdditionalStepsForSymbol(int winningSymbol, uint16_t targetMotorValueReel);
        Flasher bulb1, bulb2, bulb3;

    private:
        void calibrateReel();
        int getSymbolsIndex(int currentPos);
        uint16_t* getReelWinSymbols(long currentIndex);
        void spinningLightEffect();

        VirtualDelay lightEffectDelay;
        VirtualDelay motorDelay;
        AccelStepper stepper;
        int sensorPin;
        int bulbPin1, bulbPin2, bulbPin3;
        unsigned long previousMillis;
        int lightEffectPosition = 0;
        int effectWaterfall[4][3] = { {1,0,0}, {0,1,0}, {0,0,1}, {0,0,0} };
        bool allLightsOn = true;
        int reelIndex;
        int *reelSymbols;
        int reelSymbolsLength;
        // Flasher bulb1, bulb2, bulb3;
        int lightsOn[3] = {1, 1, 1};
        int lightsOff[3] = {0, 0, 0};
        int lightsStatus[3] = {1, 1, 1}; // possible values: 0 - off, 1 - permanent on, 2 - flash
        uint16_t reelWinSymbols[3] = {0, 0, 0};

        // int speed = 120;
        int speed = 80;
        int acceleration = 300;
        unsigned long motorStopTime = 300;
        bool motorWasRunning = false;
};

#endif