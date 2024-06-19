#include <Arduino.h>


// Define the constants for the win amounts for different symbols
// Bell, Plum, Orange, Lemon and Cherry payout is the same even in higher multiwin bets
const int WIN_2_0[] = {10, 10, 20, 20, 40, 60, 80, 100, 150};
const int WIN_5_0[] = {25, 25, 50, 50, 100, 150, 200, 250, 375};
const int WIN_5_5[] = {25, 25, 50, 50, 100, 200, 300, 400, 750};

enum Symbol {
    CHERRY,
    LEMON,
    ORANGE,
    PLUM,
    BELL,
    GRAPES,
    MELON,
    SEVEN,
    STAR
};

int calculateWinAmount(Symbol symbol, int standardBet, int multiwinBet) {
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

void setup() {
    Serial.begin(9600);
}

void loop() {
    int standardBet = 5; // Example with standard bet 5
    for (int multiwinBet = 0; multiwinBet <= 95; multiwinBet += 5) {
        int winStar = calculateWinAmount(STAR, standardBet, multiwinBet);
        int winSeven = calculateWinAmount(SEVEN, standardBet, multiwinBet);
        int winMelon = calculateWinAmount(MELON, standardBet, multiwinBet);
        int winGrapes = calculateWinAmount(GRAPES, standardBet, multiwinBet);
        int winBell = calculateWinAmount(BELL, standardBet, multiwinBet);
        int winPlum = calculateWinAmount(PLUM, standardBet, multiwinBet);
        int winOrange = calculateWinAmount(ORANGE, standardBet, multiwinBet);
        int winLemon = calculateWinAmount(LEMON, standardBet, multiwinBet);
        int winCherry = calculateWinAmount(CHERRY, standardBet, multiwinBet);

        Serial.print("Multiwin Bet: ");
        Serial.print(multiwinBet);
        Serial.print(" | Star: ");
        Serial.print(winStar);
        Serial.print(" | Seven: ");
        Serial.print(winSeven);
        Serial.print(" | Melon: ");
        Serial.print(winMelon);
        Serial.print(" | Grapes: ");
        Serial.print(winGrapes);
        Serial.print(" | Bell: ");
        Serial.print(winBell);
        Serial.print(" | Plum: ");
        Serial.print(winPlum);
        Serial.print(" | Orange: ");
        Serial.print(winOrange);
        Serial.print(" | Lemon: ");
        Serial.print(winLemon);
        Serial.print(" | Cherry: ");
        Serial.println(winCherry);

        delay(1000); // Delay for readability
    }
}
