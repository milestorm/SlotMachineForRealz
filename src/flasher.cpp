#include "flasher.h"

Flasher::Flasher(int ledPin, long OnTime, long OffTime) {
    this->ledPin = ledPin;
    pinMode(this->ledPin, OUTPUT);

    this->OnTime = OnTime;
    this->OffTime = OffTime;

    ledState = LOW;
    previousMillis = 0;

    flashCounter = false;
    flashCount = 0;
}

/*
Flasher watcher. Must be in loop function.
*/
void Flasher::tick() {
    if (updateFlshr == true) {
        if (flashCounter && flashCount == 0) {
            flashCounter = false;
            updateFlshr = false;
        }
        // check to see if it's time to change the state of the LED
        unsigned long currentMillis = millis();

        if ((ledState == HIGH) && (currentMillis - previousMillis >= OnTime)) {
            ledState = LOW;  // Turn it off
            previousMillis = currentMillis;  // Remember the time
            digitalWrite(ledPin, ledState);  // Update the actual LED

            if (flashCounter) {
                Serial.println(flashCount);
                flashCount--;
            }
        } else if ((ledState == LOW) && (currentMillis - previousMillis >= OffTime)) {
            ledState = HIGH;  // turn it on
            previousMillis = currentMillis;   // Remember the time
            digitalWrite(ledPin, ledState);	  // Update the actual LED
        }
    }
}

/*
Sets flashing interval
*/
void Flasher::setInterval(long OnTime, long OffTime) {
    this->OnTime = OnTime;
    this->OffTime = OffTime;
}

/*
Starts flashing of the light
*/
void Flasher::flashingOn() {
    updateFlshr = true;
}

void Flasher::flashForCount(int flashFor = 5) {
    updateFlshr = true;
    flashCounter = true;
    flashCount = flashFor;
}

// void Flasher::flashForCountSync(int count = 5) {
//     for (int i = 0; i < count; i++) {
//         digitalWrite(ledPin, HIGH);
//         delay(OnTime);
//         digitalWrite(ledPin, LOW);
//         delay(OffTime);
//     }
// }

/*
Sets the light permanently on
*/
void Flasher::permanentOn() {
    updateFlshr = false;
    isOn = true;
    digitalWrite(ledPin, HIGH);
}

/*
Sets the light off. Whenever is flashing or permanently on
*/
void Flasher::off() {
    updateFlshr = false;
    isOn = false;
    ledState = LOW;
    digitalWrite(ledPin, LOW);
}

/*
Is led flashing?
*/
bool Flasher::isFlashing() {
    return updateFlshr;
}

/*
Is led permanently on?
*/
bool Flasher::isPermanentOn() {
    return isOn;
}

/*
Sets the light by status
0: off
1: permanent on
2: flash
*/
void Flasher::setStatus(int statusIndex) {
    switch (statusIndex) {
        case 0 :
            off();
            break;
        case 1 :
            permanentOn();
            break;
        case 2 :
            flashingOn();
            break;
        default :
            off();
    }
}
