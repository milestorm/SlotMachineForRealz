#include <Arduino.h>
#include <OneButton.h>
#include <Samsung_16LF01_VFD.h>
#include <AccelStepper.h>
#include <Entropy.h>
#include <EEPROM.h>

#include "config.h"
#include "reel.h"
#include "vfdcontrol.h"
#include "tools.h"

// Scope definitions
void reelAndVfdLoop();

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
const int reelLength = 24;
int reelSymbols1[reelLength] = {8, 1, 1, 1, 6, 4, 4, 4, 0, 0, 0, 2, 2, 2, 3, 3, 3, 7, 1, 0, 1, 0, 5, 5};
int reelSymbols2[reelLength] = {8, 3, 3, 3, 5, 3, 2, 2, 2, 1, 1, 7, 1, 1, 1, 6, 6, 1, 0, 0, 0, 0, 4, 4};
int reelSymbols3[reelLength] = {8, 2, 0, 0, 0, 5, 5, 3, 3, 3, 7, 6, 1, 1, 1, 0, 1, 0, 0, 4, 4, 2, 2, 2};

const long flashOnTime = 100;
const long flashOffTime = 100;

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

Samsung_16LF01_VFD vfdisplay(VFD_SCLK_PIN, VFD_DATA_PIN, VFD_RST_PIN);
Vfdcontrol vfd(vfdisplay);


// HELPERS
// ---------------------
uint16_t targetMotorValueReel1;
uint16_t targetMotorValueReel2;
uint16_t targetMotorValueReel3;

uint16_t* futureSymbols1;
uint16_t* futureSymbols2;
uint16_t* futureSymbols3;

// bool textWritten = false;

int spinCounter = 0;
const int numSlots = 10; // Number of slots for wear EEPROM leveling

// Balances and wagers
int balance = 200; // current money inserted
int multiwinBalance = 500; // current money won and sent to multiwin
int standardBet = 5; // bet, which decrement each spin from "balance"
int multiwinBet = 10; // bet, which decrement each spin from "multiwinBalance"

long totalWagered = 0; // total historical wagered amount (standard + multiwin)
long totalPaidOut = 0; // total historical won amount
float targetRTP = 95.0; // target long-term return to player

uint8_t calculateChecksum(long totalWagered, long totalPaidOut) {
    uint8_t checksum = 0;
    checksum ^= (totalWagered >> 24) & 0xFF;
    checksum ^= (totalWagered >> 16) & 0xFF;
    checksum ^= (totalWagered >> 8) & 0xFF;
    checksum ^= totalWagered & 0xFF;
    checksum ^= (totalPaidOut >> 24) & 0xFF;
    checksum ^= (totalPaidOut >> 16) & 0xFF;
    checksum ^= (totalPaidOut >> 8) & 0xFF;
    checksum ^= totalPaidOut & 0xFF;
    return checksum;
}

void saveToEEPROM(long totalWagered, long totalPaidOut, int slot) {
    int baseAddress = slot * (2 * sizeof(long) + sizeof(uint8_t));
    EEPROM.put(baseAddress, totalWagered);   // Save totalWagered
    EEPROM.put(baseAddress + sizeof(long), totalPaidOut); // Save totalPaidOut
    uint8_t checksum = calculateChecksum(totalWagered, totalPaidOut);
    EEPROM.put(baseAddress + 2 * sizeof(long), checksum); // Save checksum
}

bool loadFromEEPROM(long &totalWagered, long &totalPaidOut, int slot) {
    int baseAddress = slot * (2 * sizeof(long) + sizeof(uint8_t));
    EEPROM.get(baseAddress, totalWagered);   // Load totalWagered
    EEPROM.get(baseAddress + sizeof(long), totalPaidOut); // Load totalPaidOut
    uint8_t savedChecksum;
    EEPROM.get(baseAddress + 2 * sizeof(long), savedChecksum); // Load saved checksum
    uint8_t calculatedChecksum = calculateChecksum(totalWagered, totalPaidOut);
    return savedChecksum == calculatedChecksum;
}

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

void allLEDsOff() {
    reel1.bulb1.off();
    reel1.bulb2.off();
    reel1.bulb3.off();
    reel2.bulb1.off();
    reel2.bulb2.off();
    reel2.bulb3.off();
    reel3.bulb1.off();
    reel3.bulb2.off();
    reel3.bulb3.off();
}

void allLEDsOn() {
    reel1.bulb1.permanentOn();
    reel1.bulb2.permanentOn();
    reel1.bulb3.permanentOn();
    reel2.bulb1.permanentOn();
    reel2.bulb2.permanentOn();
    reel2.bulb3.permanentOn();
    reel3.bulb1.permanentOn();
    reel3.bulb2.permanentOn();
    reel3.bulb3.permanentOn();
}

void flashLEDs(int times, int delayTime, Flasher& led1, Flasher& led2, Flasher& led3) {
    allLEDsOff();
    for (int i = 0; i < times; i++) {
        led1.permanentOn();
        led2.permanentOn();
        led3.permanentOn();
        delay(delayTime);
        led1.off();
        led2.off();
        led3.off();
        delay(delayTime);
    }
}

bool isWinner(uint16_t* symbols1, uint16_t* symbols2, uint16_t* symbols3) {
    if (symbols1[0] == symbols2[0] && symbols2[0] == symbols3[0]) {
        return true;
    }
    if (symbols1[1] == symbols2[1] && symbols2[1] == symbols3[1]) {
        return true;
    }
    if (symbols1[2] == symbols2[2] && symbols2[2] == symbols3[2]) {
        return true;
    }
    if (symbols1[0] == symbols2[1] && symbols2[1] == symbols3[2]) {
        return true;
    }
    if (symbols1[2] == symbols2[1] && symbols2[1] == symbols3[0]) {
        return true;
    }
    return false;
}

int calculateWinnings(uint16_t* symbols1, uint16_t* symbols2, uint16_t* symbols3, int standardBet, int multiwinBet) {
    int payout = 0;
    int winnings = 0;

    // Check horizontal lines
    if (symbols1[0] == symbols2[0] && symbols2[0] == symbols3[0]) {
        Serial.println("WINNER - row 1");
        flashLEDs(6, 100, reel1.bulb1, reel2.bulb1, reel3.bulb1);
        winnings = calculateWinAmount(symbols1[0], standardBet, multiwinBet);
        vfd.counterStart(payout, payout + winnings, 20, 0);
        while (vfd.isCounting()) {reelAndVfdLoop();}
        payout += winnings;
    }
    if (symbols1[1] == symbols2[1] && symbols2[1] == symbols3[1]) {
        Serial.println("WINNER - row 2");
        flashLEDs(6, 100, reel1.bulb2, reel2.bulb2, reel3.bulb2);
        winnings = calculateWinAmount(symbols1[1], standardBet, multiwinBet);
        vfd.counterStart(payout, payout + winnings, 20, 0);
        while (vfd.isCounting()) {reelAndVfdLoop();}
        payout += winnings;
    }
    if (symbols1[2] == symbols2[2] && symbols2[2] == symbols3[2]) {
        Serial.println("WINNER - row 3");
        flashLEDs(6, 100, reel1.bulb3, reel2.bulb3, reel3.bulb3);
        winnings = calculateWinAmount(symbols1[2], standardBet, multiwinBet);
        vfd.counterStart(payout, payout + winnings, 20, 0);
        while (vfd.isCounting()) {reelAndVfdLoop();}
        payout += winnings;
    }

    // Check diagonal lines
    // upper-left to bottom-right
    if (symbols1[0] == symbols2[1] && symbols2[1] == symbols3[2]) {
        Serial.println("WINNER - diagonal 1");
        flashLEDs(6, 100, reel1.bulb1, reel2.bulb2, reel3.bulb3);
        winnings = calculateWinAmount(symbols1[0], standardBet, multiwinBet);
        vfd.counterStart(payout, payout + winnings, 20, 0);
        while (vfd.isCounting()) {reelAndVfdLoop();}
        payout += winnings;
    }
    // bottom-left to upper-right
    if (symbols1[2] == symbols2[1] && symbols2[1] == symbols3[0]) {
        Serial.println("WINNER - diagonal 2");
        flashLEDs(6, 100, reel1.bulb3, reel2.bulb2, reel3.bulb1);
        winnings = calculateWinAmount(symbols1[2], standardBet, multiwinBet);
        vfd.counterStart(payout, payout + winnings, 20, 0);
        while (vfd.isCounting()) {reelAndVfdLoop();}
        payout += winnings;
    }

    allLEDsOn();

    return payout;
}

// Function to find the next index containing the winning symbol
int findNextIndex(int currentIndex, int winningSymbol, int reelSymbols[]) {
    for (int i = 0; i < reelLength; i++) {
        int newIndex = (currentIndex + i) % reelLength;
        if (reelSymbols[newIndex] == winningSymbol) {
            return newIndex;
        }
    }
    return currentIndex; // If no other index is found, return the current index
}

int getSymbolsIndex(int currentPos) {
    return ((currentPos - CALIBRATION_CONSTANT) % NUM_STEPS) / STEPS_PER_VALUE;
}

void adjustReelsForWin(uint16_t& targetMotorValueReel1, uint16_t& targetMotorValueReel2, uint16_t& targetMotorValueReel3) {
    float currentRTP = (totalPaidOut * 100.0) / totalWagered;
    float adjustmentFactor = (targetRTP - currentRTP) / targetRTP;

    int winningSymbol;
    // Define a baseline for when we decide to force a loss
    float lossThreshold = 0.5; // 50% chance to force a loss

    // Increase loss probability if the player has high credits
    if (balance + multiwinBalance > 500) {
        lossThreshold += 0.2; // Increase to 30% if credits are high
    }

    // Generate a random value to decide whether to force a loss
    float lossProbability = Entropy.random(0, 100) / 100.0;

    Serial.print("adjustmentFactor: ");
    Serial.println(adjustmentFactor);
    Serial.print("lossProbability: ");
    Serial.println(lossProbability);
    Serial.print("lossThreshold: ");
    Serial.println(lossThreshold);

    if (lossProbability < lossThreshold) {
        Serial.println("* Reels NOT adjusted *");
        return; // lose, so do not adjust a thang
        // winningSymbol = Entropy.random(0, 2); // Force a low payout symbol
    } else {
        if (adjustmentFactor > 0.5) {
            winningSymbol = Entropy.random(6, 8); // High payout symbol (6, 7, 8)
        } else if (adjustmentFactor > 0.2) {
            winningSymbol = Entropy.random(3, 5); // Moderate payout symbol (3, 4, 5)
        } else {
            winningSymbol = Entropy.random(0, 2); // Low payout symbol (0, 1, 2)
        }
    }

    // Random line to start the symbol with
    int rndNumOfLine = Entropy.random(2) - 1;
    Serial.print("rndNumOfLine: ");
    Serial.println(rndNumOfLine);

	Serial.print("Winning Symbol: ");
	Serial.println(winningSymbol);

    // Calculate the additional steps needed for each reel
    int additionalSteps1 = reel1.calculateAdditionalStepsForSymbol(winningSymbol, rndNumOfLine, targetMotorValueReel1);
    int additionalSteps2 = reel2.calculateAdditionalStepsForSymbol(winningSymbol, rndNumOfLine, targetMotorValueReel2);
    int additionalSteps3 = reel3.calculateAdditionalStepsForSymbol(winningSymbol, rndNumOfLine, targetMotorValueReel3);

    // Adjust the target motor values
    targetMotorValueReel1 += additionalSteps1;
    targetMotorValueReel2 += additionalSteps2;
    targetMotorValueReel3 += additionalSteps3;
}

void startButtonFn() {
	// kdyz se reel toci, tak nemoznost zmacknout button
	if (reel1.isRunning() || reel2.isRunning() || reel3.isRunning()) {
		return;
	}

	int currentWager = standardBet + multiwinBet;
    if (balance < currentWager) {
		// TODO mozna rozdelit standard a multiwin
        Serial.println("Insufficient Balance!");
        return;
    }

	balance -= standardBet;
    multiwinBalance -= multiwinBet;

    totalWagered += currentWager;

    vfd.clear();
    vfd.printNumberTo(balance, 2);

	targetMotorValueReel1 = Entropy.random(96, 140);
	targetMotorValueReel2 = Entropy.random(10, 72);
	targetMotorValueReel3 = Entropy.random(8, 48);

	targetMotorValueReel2 = targetMotorValueReel1 + targetMotorValueReel2;
	targetMotorValueReel3 = targetMotorValueReel2 + targetMotorValueReel3;

	 // Adjust reels for winnable outcomes if needed
    float currentRTP = (totalPaidOut * 100.0) / totalWagered;
    if (currentRTP < targetRTP) {
		Serial.println("* Adjusting reels for winning! *");
        adjustReelsForWin(targetMotorValueReel1, targetMotorValueReel2, targetMotorValueReel3);
    }

	futureSymbols1 = reel1.getSymbolsAfterSpin(targetMotorValueReel1);
	futureSymbols2 = reel2.getSymbolsAfterSpin(targetMotorValueReel2);
	futureSymbols3 = reel3.getSymbolsAfterSpin(targetMotorValueReel3);

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

	reel1.spin(targetMotorValueReel1);
	reel2.spin(targetMotorValueReel2);
	reel3.spin(targetMotorValueReel3);
	// return symbols are at reel2.reelWinSymbols

	// Wait for reels to stop
    while (reel1.isRunning() || reel2.isRunning() || reel3.isRunning()) {
        reel1.tick();
        reel2.tick();
        reel3.tick();
    }

	// Calculate and pay out winnings
    int winnings = calculateWinnings(futureSymbols1, futureSymbols2, futureSymbols3, standardBet, multiwinBet);
    // TODO pocitat s multiwinem
    balance += winnings;
    totalPaidOut += winnings;

    // Display results
    Serial.print("Balance: ");
    Serial.println(balance);
    Serial.print("Multiwin Balance: ");
    Serial.println(multiwinBalance);
    Serial.print("Winnings: ");
    Serial.println(winnings);
    float newRTP = (totalPaidOut * 100.0) / totalWagered;
    Serial.print("RTP: ");
    Serial.print(newRTP, 2);
    Serial.println("%");

    Serial.print("totalWagered: ");
    Serial.println(totalWagered);
    Serial.print("totalPaidOut: ");
    Serial.println(totalPaidOut);

    Serial.println("__________________");

    spinCounter++;
    if (spinCounter >= 10) {
        int slot = spinCounter % numSlots; // Rotate through slots
        saveToEEPROM(totalWagered, totalPaidOut, slot);
        Serial.print("Saved to EEPROM slot ");
        Serial.println(slot);
        spinCounter = 0;
    }
}

// =====================================================================

void setup() {
	if (DEBUG) {
		Serial.begin(9600);
		Serial.println("Booting up...");
	}

    bool dataLoaded = false;
    for (int slot = 0; slot < numSlots; slot++) {
        if (loadFromEEPROM(totalWagered, totalPaidOut, slot)) {
            Serial.print("Loaded Total Wagered from slot ");
            Serial.print(slot);
            Serial.print(": ");
            Serial.println(totalWagered);
            Serial.print("Loaded Total Paid Out from slot ");
            Serial.print(slot);
            Serial.print(": ");
            Serial.println(totalPaidOut);
            dataLoaded = true;
            break;
        }
    }

    if (!dataLoaded) {
        // Data is invalid, initialize with default values
        totalWagered = 0;
        totalPaidOut = 0;
        Serial.println("EEPROM data invalid in all slots, initializing with default values.");
    }

	Entropy.initialize();

	vfd.init();

	reel1.init();
	reel2.init();
	reel3.init();

	startButton.attachClick(startButtonFn);

    if (DEBUG) {
		Serial.begin(9600);
		Serial.println("Fruit Machine is ready.");
	}

    vfd.clear();
    vfd.printNumberTo(balance, 2);
}

void reelAndVfdLoop() {
    reel1.tick();
	reel2.tick();
	reel3.tick();

	vfd.update();
}

void loop() {
	startButton.tick();

    reelAndVfdLoop();

	// vfd.print(millis() / 1000);
	// vfd.println(" seconds");
	// delay(1000);
}