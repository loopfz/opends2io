#include "Arduino.h"
#include "DB15Decoder.h"
#include "constants.h"
#include "ds2.h"

DB15Decoder::DB15Decoder(DS2& ds2, RGBLed& errLed) :
_ds2(ds2), _errLed(errLed)
{
}

void DB15Decoder::init() {
    // set all switch pins to INPUT_PULLUP
    // they will be pulled high when the switch is open (not pressed)
    // so we can test later if they are LOW because the closed switch is grounding them
    pinMode(P1UP_PIN, INPUT_PULLUP);
    pinMode(P1DOWN_PIN, INPUT_PULLUP);
    pinMode(P1LEFT_PIN, INPUT_PULLUP);
    pinMode(P1RIGHT_PIN, INPUT_PULLUP);
    pinMode(P1B1_PIN, INPUT_PULLUP);
    pinMode(P1B2_PIN, INPUT_PULLUP);
    pinMode(P1B3_PIN, INPUT_PULLUP);
    pinMode(P1START_PIN, INPUT_PULLUP);
    pinMode(P1COIN_PIN, INPUT_PULLUP);
    pinMode(P2UP_PIN, INPUT_PULLUP);
    pinMode(P2DOWN_PIN, INPUT_PULLUP);
    pinMode(P2LEFT_PIN, INPUT_PULLUP);
    pinMode(P2RIGHT_PIN, INPUT_PULLUP);
    pinMode(P2B1_PIN, INPUT_PULLUP);
    pinMode(P2B2_PIN, INPUT_PULLUP);
    pinMode(P2B3_PIN, INPUT_PULLUP);
    pinMode(P2START_PIN, INPUT_PULLUP);
    pinMode(P2COIN_PIN, INPUT_PULLUP);
}

void DB15Decoder::reset() {
}

bool DB15Decoder::needReset() {
    return false;
}

void DB15Decoder::process() {

    unsigned char   p1b1 = 0, p1b2 = 0,
                    p2b1 = 0, p2b2 = 0;

    unsigned char coins = 0;

    if (!digitalRead(P1UP_PIN)) {
        p1b1 |= (1 << 5);
    }
    if (!digitalRead(P1DOWN_PIN)) {
        p1b1 |= (1 << 4);
    }
    if (!digitalRead(P1LEFT_PIN)) {
        p1b1 |= (1 << 3);
    }
    if (!digitalRead(P1RIGHT_PIN)) {
        p1b1 |= (1 << 2);
    }
    if (!digitalRead(P1B1_PIN)) {
        p1b1 |= (1 << 1);
    }
    if (!digitalRead(P1B2_PIN)) {
        p1b1 |= 1;
    }
    if (!digitalRead(P1B3_PIN)) {
        p1b2 |= (1 << 7);
    }
    if (!digitalRead(P1START_PIN)) {
        p1b1 |= (1 << 7);
    }
    if (!digitalRead(P1COIN_PIN)) {
        coins++;
    }

    if (!digitalRead(P2UP_PIN)) {
        p2b1 |= (1 << 5);
    }
    if (!digitalRead(P2DOWN_PIN)) {
        p2b1 |= (1 << 4);
    }
    if (!digitalRead(P2LEFT_PIN)) {
        p2b1 |= (1 << 3);
    }
    if (!digitalRead(P2RIGHT_PIN)) {
        p2b1 |= (1 << 2);
    }
    if (!digitalRead(P2B1_PIN)) {
        p2b1 |= (1 << 1);
    }
    if (!digitalRead(P2B2_PIN)) {
        p2b1 |= 1;
    }
    if (!digitalRead(P2B3_PIN)) {
        p2b2 |= (1 << 7);
    }
    if (!digitalRead(P2START_PIN)) {
        p2b1 |= (1 << 7);
    }
    if (!digitalRead(P2COIN_PIN)) {
        coins++;
    }

    if (this->_ds2.coinInputs(0, coins, 0, 0)) {
        this->_ds2.playerInputs(p1b1, p1b2, p2b1, p2b2);
    }
}
