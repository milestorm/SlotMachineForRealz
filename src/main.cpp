#include "config.h"
#include <Arduino.h>
#include <OneButton.h>
#include <Samsung_16LF01_VFD.h>
#include <AccelStepper.h>

#include "reel.h"
#include "tools.h"


OneButton startButton(5, true); 



int reelSymbols2[24] = {8,3,3,3,5,3,2,2,2,1,1,7,1,1,1,6,6,1,0,0,0,0,4,4}; // hvezdicka je prvni, pak smerem k care
int reelLength = 24;



//bool textWritten = false;

const byte sensorPin = 2;


AccelStepper stepper(AccelStepper::FULL4WIRE, 8, 9, 10, 11);
Reel reel1(1, stepper, sensorPin, reelSymbols2, reelLength);




/********************
  VFD display Pin config:
  
  SCLK = pin 3
  RST  = pin 2
  DATA = pin 4
*********************/
//Samsung_16LF01_VFD vfd(3, 4, 2);




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
  if (DEBUG) {
    if (!stepper.isRunning()) {
      if (!textWritten) {
        Serial.print("LANDED: ");
        Serial.println(getSymbolsIndex(stepper.currentPosition()) );
        textWritten = true;
      }
      
    } else {
      textWritten = false;
    }
  }
  
}
*/


void setup()
{
  if (DEBUG) {
    Serial.begin(9600);
    Serial.println("Booting up...");
  }
  
  reel1.init();

  startButton.attachClick(startButtonFn);

  // Init the display, 16 digits and 5/31 of brightness
  //vfd.begin(16, 20);
  //vfd.print("hello world");
  //delay(3000);
  //vfd.clear();
}

void loop()
{
  startButton.tick();

  reel1.tick();
  //reelWatcher();

  //vfd.print(millis() / 1000);
  //vfd.println(" seconds");
  //delay(1000);
}