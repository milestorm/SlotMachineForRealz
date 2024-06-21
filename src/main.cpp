#include <Arduino.h>
#include <OneButton.h>
#include <Samsung_16LF01_VFD.h>
#include <AccelStepper.h>
#include <Entropy.h>

#include "config.h"
#include "reel.h"
#include "vfdcontrol.h"
#include "tools.h"

// REELS DEFINITIONS
// ---------------------------------

// Define the constants for the win amounts for different symbols
// Bell, Plum, Orange, Lemon and Cherry payout is the same even in higher multiwin bets
const int WIN_2_0[] = {10, 10, 20, 20, 40, 60, 80, 100, 150};
const int WIN_5_0[] = {25, 25, 50, 50, 100, 150, 200, 250, 375};
const int WIN_5_5[] = {25, 25, 50, 50, 100, 200, 300, 400, 750};

//              0       1      2       3     4     5       6      7      8
// enum Symbol {CHERRY, LEMON, ORANGE, PLUM, BELL, GRAPES, MELON, SEVEN, STAR};

// reels are displayed from star as first symbol, and then as they appear in downwards motion of reel
// Star is first, then towards the glueing line of reel, so upwards
int reelSymbols1[24] = {8, 1, 1, 1, 6, 4, 4, 4, 0, 0, 0, 2, 2, 2, 3, 3, 3, 7, 1, 0, 1, 0, 5, 5};
int reelSymbols2[24] = {8, 3, 3, 3, 5, 3, 2, 2, 2, 1, 1, 7, 1, 1, 1, 6, 6, 1, 0, 0, 0, 0, 4, 4};
int reelSymbols3[24] = {8, 2, 0, 0, 0, 5, 5, 3, 3, 3, 7, 6, 1, 1, 1, 0, 1, 0, 0, 4, 4, 2, 2, 2};
int reelLength = 24;

const long flashOnTime = 400;
const long flashOffTime = 300;

// REEL 1
const int sensorPin1 = REEL_1__IR_SENSOR;
AccelStepper stepper1(AccelStepper::FULL4WIRE, REEL_1__MOTOR_A, REEL_1__MOTOR_B, REEL_1__MOTOR_C, REEL_1__MOTOR_D);
Flasher reel1_light1(REEL_1__LIGHT_1, flashOnTime, flashOffTime);
Flasher reel1_light2(REEL_1__LIGHT_2, flashOnTime, flashOffTime);
Flasher reel1_light3(REEL_1__LIGHT_3, flashOnTime, flashOffTime);
Reel reel1(1, stepper1, reelSymbols1, reelLength, sensorPin1, reel1_light1, reel1_light2, reel1_light3);
// REEL 2
const int sensorPin2 = REEL_2__IR_SENSOR;
AccelStepper stepper2(AccelStepper::FULL4WIRE, REEL_2__MOTOR_A, REEL_2__MOTOR_B, REEL_2__MOTOR_C, REEL_2__MOTOR_D);
Flasher reel2_light1(REEL_2__LIGHT_1, flashOnTime, flashOffTime);
Flasher reel2_light2(REEL_2__LIGHT_2, flashOnTime, flashOffTime);
Flasher reel2_light3(REEL_2__LIGHT_3, flashOnTime, flashOffTime);
Reel reel2(2, stepper2, reelSymbols2, reelLength, sensorPin2, reel2_light1, reel2_light2, reel2_light3);
// REEL 3
const int sensorPin3 = REEL_3__IR_SENSOR;
AccelStepper stepper3(AccelStepper::FULL4WIRE, REEL_3__MOTOR_A, REEL_3__MOTOR_B, REEL_3__MOTOR_C, REEL_3__MOTOR_D);
Flasher reel3_light1(REEL_3__LIGHT_1, flashOnTime, flashOffTime);
Flasher reel3_light2(REEL_3__LIGHT_2, flashOnTime, flashOffTime);
Flasher reel3_light3(REEL_3__LIGHT_3, flashOnTime, flashOffTime);
Reel reel3(3, stepper3, reelSymbols3, reelLength, sensorPin3, reel3_light1, reel3_light2, reel3_light3);

// BUTTONS DEFINITIONS
// ---------------------------------

OneButton startButton(BUTT_START, true);


// DISPLAY DEFINITIONS
// ---------------------------------

// Samsung_16LF01_VFD vfdisplay(VFD_SCLK_PIN, VFD_DATA_PIN, VFD_RST_PIN);
// Vfdcontrol vfd(vfdisplay);


// HELPERS
// ---------------------
uint16_t targetValueReel1;
uint16_t targetValueReel2;
uint16_t targetValueReel3;

uint16_t* futureSymbols1;
uint16_t* futureSymbols2;
uint16_t* futureSymbols3;

// bool textWritten = false;

/**
 * Calculates winning amount
 * @param symbol {int} symbol Symbol number (0 lowest [cherry], 8 highest [star])
 * @param standardBet {int} standard bet (2, 5)
 * @param multiwinBet {int} bet from multiwin (0, 5, 10, 15 .. 95)
 */
int calculateWinAmount(int symbol, int standardBet, int multiwinBet) {
    if (standardBet == 2) {
        return WIN_2_0[symbol];
    } else if (standardBet == 5) {
        if (multiwinBet == 0) {
            return WIN_5_0[symbol];
        } else {
            int increment = (WIN_5_5[symbol] - WIN_5_0[symbol]) / 5;
            return WIN_5_0[symbol] + multiwinBet * increment;
        }
    }
    return 0; // Default return if bets are not 2 or 5
}

void startButtonFn() {
	// TODO: kdyz se reel toci, tak nemoznost zmacknout button
	targetValueReel1 = Entropy.random(96, 160);
	targetValueReel2 = Entropy.random(2, 72);
	targetValueReel3 = Entropy.random(2, 48);

	targetValueReel2 = targetValueReel1 + targetValueReel2;
	targetValueReel3 = targetValueReel2 + targetValueReel3;

	futureSymbols1 = reel1.getSymbolsAfterSpin(targetValueReel1);
	futureSymbols2 = reel2.getSymbolsAfterSpin(targetValueReel2);
	futureSymbols3 = reel3.getSymbolsAfterSpin(targetValueReel3);

	Serial.println("futureSymbols:");
	Serial.print(futureSymbols1[0]);
	Serial.print(" - ");
	Serial.print(futureSymbols2[0]);
	Serial.print(" - ");
	Serial.println(futureSymbols3[0]);
	Serial.print(futureSymbols1[1]);
	Serial.print(" - ");
	Serial.print(futureSymbols2[1]);
	Serial.print(" - ");
	Serial.println(futureSymbols3[1]);
	Serial.print(futureSymbols1[2]);
	Serial.print(" - ");
	Serial.print(futureSymbols2[2]);
	Serial.print(" - ");
	Serial.println(futureSymbols3[2]);

	reel1.spin(targetValueReel1);
	reel2.spin(targetValueReel2);
	reel3.spin(targetValueReel3);
	// return symbols are at reel2.reelWinSymbols
}

/*
void reelWatcher() {
	if (!stepper.isRunning()) {

	} else {
	  
	}

}
*/


void setup() {
	if (DEBUG) {
		Serial.begin(9600);
		Serial.println("Booting up...");
	}

	Entropy.initialize();

	// vfd.init();

	reel1.init();
	reel2.init();
	reel3.init();

	startButton.attachClick(startButtonFn);

}

void loop() {
	startButton.tick();

	// vfd.tick();

	reel1.tick();
	reel2.tick();
	reel3.tick();
	// reelWatcher();

	// vfd.print(millis() / 1000);
	// vfd.println(" seconds");
	// delay(1000);
}