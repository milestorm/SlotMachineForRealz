#include "reel.h"
#include "flasher.h"

/*
Class initialization with motor, calibration sensor and light bulbs
@param reelIndex {int} Index of position in the slot machine
@param stepper {AccelStepper} Motor
@param *reelSymbols {int} Array of reel values
@param reelSymbolsLength {int} Length of reel array
@param sensorPin {int} Optical sensor pin
@param bulb1 {Flasher} Light bulb 1
@param bulb2 {Flasher} Light bulb 2
@param bulb3 {Flasher} Light bulb 3
*/
Reel::Reel(int reelIndex, AccelStepper &stepper, int *reelSymbols, int reelSymbolsLength, int sensorPin, Flasher &bulb1, Flasher &bulb2, Flasher &bulb3) {
    this->reelIndex = reelIndex;
    this->stepper = stepper;
    this->sensorPin = sensorPin;
    this->reelSymbols = reelSymbols;
    this->reelSymbolsLength = reelSymbolsLength;
    this->bulb1 = bulb1;
    this->bulb2 = bulb2;
    this->bulb3 = bulb3;
}

/*
Initializes reel. Also calibrates reel to zero position by optical sensor on reel.
*/
void Reel::init() {
    // sets pins
    pinMode(sensorPin, INPUT);

    // sets and calibrates stepper
    stepper.setMaxSpeed(speed);
    stepper.setAcceleration(acceleration);
    calibrateReel();

    // turns off lights
    setLights(lightsStatus);
}

/*
Calibrates reel with slow speed
*/
void Reel::calibrateReel() {
    stepper.setMaxSpeed(20);
    stepper.move(NUM_STEPS);
    while(!digitalRead(sensorPin)) {
        stepper.run();
    }
    stepper.stop();
    stepper.setCurrentPosition(CALIBRATION_CONSTANT);
    stepper.setMaxSpeed(speed);
}

/*
Reel watcher. Must be in loop function.
*/
bool Reel::tick() {
    bulb1.tick();
    bulb2.tick();
    bulb3.tick();
    return stepper.run();
}

/*
Checks to see if the motor is currently running to a target
*/
bool Reel::isRunning() {
    return stepper.isRunning();
}

/*
Gets the real index of a symbol, which is ocekavan in its array
@param currentPos {long} Position of a stepper
*/
int Reel::getSymbolsIndex(long currentPos) {
    long currentPosition = ((currentPos - CALIBRATION_CONSTANT) % NUM_STEPS) / STEPS_PER_VALUE;
    return currentPosition;
}

/*
Sets the reelWinSymbols array the three symbols, which are up and down from currentIndex
@param currentIndex {long} index of steppers currentPosition increased of the future value
*/
int* Reel::getReelWinSymbols(long currentIndex) {
    // 1st
    if (currentIndex == (reelSymbolsLength - 1)) {
        reelWinSymbols[0] = reelSymbols[0];
    } else {
        reelWinSymbols[0] = reelSymbols[currentIndex + 1];
    }

    // 2nd
    reelWinSymbols[1] = reelSymbols[currentIndex];

    // 3rd
    if (currentIndex == 0) {
        reelWinSymbols[2] = reelSymbols[reelSymbolsLength - 1];
    } else {
        reelWinSymbols[2] = reelSymbols[currentIndex - 1];
    }

    return reelWinSymbols;
}

/*
Kind of hindsight of symbols which will be on reels after the spin
@param targetValue {int} for how many symbols will the reel turn
*/
int* Reel::getSymbolsAfterSpin(int targetValue) {
    int *symbArray = getReelWinSymbols(getSymbolsIndex(stepper.currentPosition() + (targetValue * STEPS_PER_VALUE)) );

    if (DEBUG) {
        Serial.print("Symbols will be: ");
        Serial.print(symbArray[0]);
        Serial.print(", ");
        Serial.print(symbArray[1]);
        Serial.print(", ");
        Serial.println(symbArray[2]);
    }

    return symbArray;
}

/*
Physically spin the reel
@param targetValue {int} for how many symbols will the reel turn
*/
void Reel::spin(int targetValue) {
    stepper.move(targetValue * STEPS_PER_VALUE);

    if (DEBUG) {
        Serial.print(reelIndex);
        Serial.print(". reel is going to: ");
        Serial.println(targetValue);
    }

}

/*
Sets lights array
*/
void Reel::setLights(int *bulbArray) {
    bulb1.setStatus(bulbArray[0]);
    bulb2.setStatus(bulbArray[1]);
    bulb3.setStatus(bulbArray[2]);
}