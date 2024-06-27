#include "reel.h"

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
    this->previousMillis = 0;
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
void Reel::tick() {
    bulb1.tick();
    bulb2.tick();
    bulb3.tick();

    // spinning waterfall effect
    if (LIGHT_EFFECT_ON && isRunning()) {
        allLightsOn = false;
        spinningLightEffect();
    } else if (LIGHT_EFFECT_ON && !isRunning()) {
        if (!allLightsOn) {
            allLightsOn = true;
            setLights(lightsOn);
        }
    }

    if (stepper.isRunning()) {
        stepper.enableOutputs();
        stepper.run();
        motorWasRunning = true;
        previousMillis = millis();
    } else {
        unsigned long currentMillis = millis();
        if (motorWasRunning && currentMillis - previousMillis >= motorStopTime) {
            previousMillis = currentMillis;  // Remember the time
            stepper.stop();
            stepper.disableOutputs();
            motorWasRunning = false;
        }
    }
    // stepper.run();
}

void Reel::stopMotor() {
    stepper.stop();
    stepper.disableOutputs();
    motorWasRunning = false;
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
int Reel::getSymbolsIndex(int currentPos) {
    int currentPosition = ((currentPos - CALIBRATION_CONSTANT) % NUM_STEPS) / STEPS_PER_VALUE;
    return currentPosition;
}

/*
Sets the reelWinSymbols array the three symbols, which are up and down from currentIndex
@param currentIndex {long} index of steppers currentPosition increased of the future value
*/
uint16_t* Reel::getReelWinSymbols(long currentIndex) {
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
uint16_t* Reel::getSymbolsAfterSpin(uint16_t targetValue) {
    uint16_t *symbArray = getReelWinSymbols(getSymbolsIndex(stepper.currentPosition() + (targetValue * STEPS_PER_VALUE)) );

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

uint16_t Reel::calculateTargetValue(int targetPos) {
    int currentIndex = getSymbolsIndex(stepper.currentPosition());
    int stepsToMove = (targetPos - currentIndex + reelSymbolsLength) % reelSymbolsLength;
    return stepsToMove * STEPS_PER_VALUE;
}

int Reel::getFutureSymbolsIndex(uint16_t targetValue) {
    int currentIndex = getSymbolsIndex(stepper.currentPosition() + (targetValue * STEPS_PER_VALUE));
    return currentIndex;
}

// Function to find the next index containing the winning symbol
int Reel::findSymbolIndex(int winningSymbol, uint16_t targetMotorValueReel) {
    int currentIndex = getFutureSymbolsIndex(targetMotorValueReel);
    for (int i = 0; i < reelSymbolsLength; i++) {
        int newIndex = (currentIndex + i) % reelSymbolsLength;
        if (reelSymbols[newIndex] == winningSymbol) {
            return newIndex;
        }
    }
    return currentIndex; // If no other index is found, return the current index
}

int Reel::calculateAdditionalStepsForSymbol(int winningSymbol, uint16_t targetMotorValueReel) {
    int currentIndex = getFutureSymbolsIndex(targetMotorValueReel);
    int nextIndex = findSymbolIndex(winningSymbol, targetMotorValueReel) + 1; // TODO tady mozna udelat random mezi -1, 0 a 1. aby tot bylo vic modularni kde ti to da ten viteznej srac
    int additionalSteps = (nextIndex - currentIndex - reelSymbolsLength) % reelSymbolsLength;

    return additionalSteps;
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

/*
Does the waterfall effect with the lights. Used while reels are spinning.
*/
void Reel::spinningLightEffect() {
    lightEffectDelay.start(50);
    if (lightEffectDelay.elapsed()) {
        switch (lightEffectPosition) {
        case 0:
            setLights(effectWaterfall[0]);
            break;

        case 1:
            setLights(effectWaterfall[1]);
            break;

        case 2:
            setLights(effectWaterfall[2]);
            break;

        case 3:
            setLights(effectWaterfall[3]);
            break;

        default:
            break;
        }
        // resets the light cycle
        lightEffectPosition++;
        if (lightEffectPosition > 3) {
            lightEffectPosition = 0;
        }
    }
}