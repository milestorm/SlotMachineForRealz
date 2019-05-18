#include "config.h"
#include <Arduino.h>

/*
Library of tools useful thru all project
*/

// Seed generator from ADC pin (pins A6 and A7 on NANO)
uint32_t get_seed(int pin) {

    uint16_t aread;
    union {
        uint32_t as_uint32_t;
        uint8_t  as_uint8_t[4];
    } seed;
    uint8_t i, t;

        /* "aread" shifts 3 bits each time and the shuffle
        * moves bytes around in chunks of 8.  To ensure
        * every bit is combined with every other bit,
        * loop 3 x 8 = 24 times.
        */
    for (i = 0; i < 24; i++) {
        /* Shift three bits of A2D "noise" into aread. */
        aread <<= 3;
        aread |= analogRead(pin) & 0x7;

        /* Now shuffle the bytes of the seed
        * and xor our new set of bits onto the
        * the seed.
        */
        t = seed.as_uint8_t[0];
        seed.as_uint8_t[0] = seed.as_uint8_t[3];
        seed.as_uint8_t[3] = seed.as_uint8_t[1];
        seed.as_uint8_t[1] = seed.as_uint8_t[2];
        seed.as_uint8_t[2] = t;

        seed.as_uint32_t ^= aread;
    }

    return(seed.as_uint32_t);
}


int randomGenerator(int min, int max) { // range : [min, max)
   static bool rndGenFirst = true;
   if (rndGenFirst) {
      uint32_t seed = get_seed(ADC_SEED_PIN);

      if (DEBUG) {
        Serial.print("Using seed: ");
        Serial.println(seed);
      }

      srandom(seed); // seeding for the first time only!
      rndGenFirst = false;
   }
   return min + random() % (( max + 1 ) - min);
}