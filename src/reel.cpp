#include "reel.h"

/*
Class initialization
@param reelIndex {int} Index of position in the slot machine
@param stepper {AccelStepper} Motor
@param sensorPin {int} Optical sensor pin
@param *reelSymbols {int} Array of reel values 
@param reelSymbolsLength {int} Length of reel array
*/
Reel::Reel(int reelIndex, AccelStepper &stepper, int sensorPin, int *reelSymbols, int reelSymbolsLength) {
    this->reelIndex = reelIndex;
    this->stepper = stepper;
    this->sensorPin = sensorPin;
    this->reelSymbols = reelSymbols;
    this->reelSymbolsLength = reelSymbolsLength;
}

/*
Initializes reel. Also calibrates reel to zero position by optical sensor on reel.
*/
void Reel::init() {
    pinMode(sensorPin, INPUT);
    stepper.setMaxSpeed(speed);
    stepper.setAcceleration(acceleration);
    calibrateReel();
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

void Reel::spin(int targetValue) {
    stepper.move(targetValue * STEPS_PER_VALUE);

    if (DEBUG) {
        Serial.print(reelIndex);
        Serial.print(". reel is going to: ");
        Serial.println(targetValue);
    }
  
}