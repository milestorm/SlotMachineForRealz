#include "config.h"
#include <Arduino.h>
#include <OneButton.h>
#include <Samsung_16LF01_VFD.h>
#include <AccelStepper.h>
#include <Entropy.h>

#include "reel.h"
#include "vfdcontrol.h"
#include "tools.h"


OneButton startButton(4, true);

// Define the constants for the win amounts for different symbols
// Bell, Plum, Orange, Lemon and Cherry payout is the same even in higher multiwin bets
const int WIN_2_0[] = {10, 10, 20, 20, 40, 60, 80, 100, 150};
const int WIN_5_0[] = {25, 25, 50, 50, 100, 150, 200, 250, 375};
const int WIN_5_5[] = {25, 25, 50, 50, 100, 200, 300, 400, 750};

// enum Symbol {CHERRY, LEMON, ORANGE, PLUM, BELL, GRAPES, MELON, SEVEN, STAR};
int reelSymbols2[24] = {8,3,3,3,5,3,2,2,2,1,1,7,1,1,1,6,6,1,0,0,0,0,4,4}; // hvezdicka je prvni, pak smerem k care
int reelLength = 24;



// bool textWritten = false;

const byte sensorPin = 2; // please define this pin in config.h


AccelStepper stepper(AccelStepper::FULL4WIRE, 8, 9, 10, 11);
Flasher reel2_bulb1(5, 400, 300);
Flasher reel2_bulb2(6, 400, 300);
Flasher reel2_bulb3(7, 400, 300);
Reel reel2(1, stepper, reelSymbols2, reelLength, sensorPin, reel2_bulb1, reel2_bulb2, reel2_bulb3);

// Samsung_16LF01_VFD vfdisplay(VFD_SCLK_PIN, VFD_DATA_PIN, VFD_RST_PIN);
// Vfdcontrol vfd(vfdisplay);

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
	uint16_t targetValueReel2 = Entropy.random(96, 160);

	uint16_t* futureSymbols = reel2.getSymbolsAfterSpin(targetValueReel2);

	Serial.print("futureSymbols: ");
	Serial.print(futureSymbols[0]);
	Serial.print(", ");
	Serial.print(futureSymbols[1]);
	Serial.print(", ");
	Serial.println(futureSymbols[2]);

	reel2.spin(targetValueReel2);
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

	reel2.init();

	startButton.attachClick(startButtonFn);

}

void loop() {
	startButton.tick();

	// vfd.tick();

	reel2.tick();
	// reelWatcher();

	// vfd.print(millis() / 1000);
	// vfd.println(" seconds");
	// delay(1000);
}