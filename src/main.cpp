#include "config.h"
#include <Arduino.h>
#include <OneButton.h>
#include <Samsung_16LF01_VFD.h>
#include <AccelStepper.h>

#include "reel.h"
#include "vfdcontrol.h"
#include "tools.h"


OneButton startButton(4, true);



int reelSymbols2[24] = {8,3,3,3,5,3,2,2,2,1,1,7,1,1,1,6,6,1,0,0,0,0,4,4}; // hvezdicka je prvni, pak smerem k care
int reelLength = 24;



//bool textWritten = false;

const byte sensorPin = 2; // please define this pin in config.h


AccelStepper stepper(AccelStepper::FULL4WIRE, 8, 9, 10, 11);
Flasher reel1_bulb1(5, 400, 300);
Flasher reel1_bulb2(6, 400, 300);
Flasher reel1_bulb3(7, 400, 300);
Reel reel1(1, stepper, reelSymbols2, reelLength, sensorPin, reel1_bulb1, reel1_bulb2, reel1_bulb3);

//Samsung_16LF01_VFD vfdisplay(VFD_SCLK_PIN, VFD_DATA_PIN, VFD_RST_PIN);
//Vfdcontrol vfd(vfdisplay);



void startButtonFn() {
    int targetValue = randomGenerator(96, 160);

    int* futureSymbols = reel1.getSymbolsAfterSpin(targetValue);

    Serial.print("futureSymbols: ");
    Serial.print(futureSymbols[0]);
    Serial.print(", ");
    Serial.print(futureSymbols[1]);
    Serial.print(", ");
    Serial.println(futureSymbols[2]);

    reel1.spin(targetValue);
    // return symbols are at reel1.reelWinSymbols

}

/*
void reelWatcher() {
    if (!stepper.isRunning()) {

    } else {
      
    }

}
*/


void setup()
{
  if (DEBUG) {
    Serial.begin(9600);
    Serial.println("Booting up...");
  }

  //vfd.init();

  reel1.init();

  startButton.attachClick(startButtonFn);

}

void loop()
{
  startButton.tick();

  //vfd.tick();

  reel1.tick();
  //reelWatcher();

  //vfd.print(millis() / 1000);
  //vfd.println(" seconds");
  //delay(1000);
}