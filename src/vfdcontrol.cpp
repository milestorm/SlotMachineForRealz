#include "vfdcontrol.h"

/*
Uses the external library and extends its usability for use with the slot machine
*/
Vfdcontrol::Vfdcontrol(Samsung_16LF01_VFD &vfdisplay) {
    this->vfdisplay = vfdisplay;
}

void Vfdcontrol::init() {
    // Init the display, 16 digits and 20/31 of brightness
    vfdisplay.begin(16, 20);
    vfdisplay.clear();
}

void Vfdcontrol::printToLeft(const char* text) {
    vfdisplay.setCursor(0);
    vfdisplay.print(text);
}

void Vfdcontrol::printToCenter(const char* text) {
    uint8_t textLength = strlen(text);
    if (textLength > 16) {
        textLength = 16; // truncate if the text is too long
    }
    uint8_t startPosition = (16 - textLength) / 2;
    vfdisplay.setCursor(startPosition);
    vfdisplay.print(text);
}

void Vfdcontrol::printToRight(const char* text) {
    uint8_t textLength = strlen(text);
    if (textLength > 16) {
        textLength = 16; // truncate if the text is too long
    }
    uint8_t startPosition = 16 - textLength;
    vfdisplay.setCursor(startPosition);
    vfdisplay.print(text);
}

void Vfdcontrol::counterStart(int startValue, int endValue, int delayMillis, int counterPrintPos) {
    currentValue = startValue;
    targetValue = endValue;
    delayTime = delayMillis;
    counterPrintPosition = counterPrintPos;
    previousMillis = millis();
    increment = (startValue < endValue) ? 1 : -1;
    counting = true;
}

bool Vfdcontrol::isCounting() {
    return counting;
}

int Vfdcontrol::getCurrentValue() {
    return currentValue;
}

void Vfdcontrol::update() {
    // update counter
    if (!counting) return;

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= delayTime) {
        previousMillis = currentMillis;

        char buffer[17]; // Buffer to hold the number as a string with null terminator
        itoa(getCurrentValue(), buffer, 10); // Convert int to string

        switch (counterPrintPosition) {
        case 0:
            printToLeft(buffer);
            break;
        case 1:
            printToCenter(buffer);
            break;
        case 2:
            printToRight(buffer);
            break;

        default:
            break;
        }

        if (currentValue != targetValue) {
            currentValue += increment;
        } else {
            // Serial.println("Counting complete");
            counting = false;
        }
    }
}