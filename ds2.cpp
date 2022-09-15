#include "Arduino.h"
#include "constants.h"
#include "rgbled.h"
#include "ds2.h"

DS2::DS2(RGBLed& errLed): _errLed(errLed) {
    this->lastHeartbeat = 0;
    this->coins1 = 0;
    this->coins2 = 0;

    // init to 0xFF to make sure we send our initial state (see playerInputs function)
    this->lastp1b1 = 0xFF;
    this->lastp1b2 = 0xFF;
    this->lastp2b1 = 0xFF;
    this->lastp2b2 = 0xFF;
    this->prevlastp1b1 = 0xFF;
    this->prevlastp1b2 = 0xFF;
    this->prevlastp2b1 = 0xFF;
    this->prevlastp2b2 = 0xFF;

    this->_p1Deadline = 0;
    this->_p2Deadline = 0;
    this->_buffer_p1b1 = 0;
    this->_buffer_p1b2 = 0;
    this->_buffer_p2b1 = 0;
    this->_buffer_p2b2 = 0;
}

void readNBytes(int n, unsigned char buf[]) {

    for (int i = 0; i < n;) {
        while (!Serial.available()) {
        }
        buf[i] = (char) Serial.read();
        if (buf[i] == 0 || buf[i] == 13 || buf[i] == 10) {
            // skip \0 \r \n garbage
            continue;
        }
        i++;
    }
}

bool readSequence(unsigned char seq[], int n, unsigned char buf[]) {

    readNBytes(n, buf);

    for (int i = 0; i < n; i++) {
        if (seq[i] != buf[i]) {
            return false;
        }
    }

    return true;
}

void DS2::heartbeat() {

    unsigned long now = millis();

    if (this->lastHeartbeat == 0 || (now - this->lastHeartbeat) > DS2_DELAY_HEARTBEAT) {
        Serial.write("!!!", 3);
        this->lastHeartbeat = now;
    }
}

bool DS2::init() {

    unsigned char buf[4] = {0};

    unsigned char seq[] = { '@', '*', '*', '*' };
    if (!readSequence(seq, sizeof(seq), buf)) {
        // unexpected sequence
        this->_errLed.SlowBlinkLed(LED_YELLOW, LED_OFF, 60000);
        return false; // returning here should block the game startup
    }
    Serial.write(buf, 4);

    unsigned char seq2[] = { '&', '*', '*', '*' };
    if (!readSequence(seq2, sizeof(seq2), buf)) {
        // unexpected sequence
        this->_errLed.BlinkLed(LED_YELLOW, LED_WHITE, 60000);
        return false; // returning here should block the game startup
    }
    Serial.write(DS2_UNIQUE_ID, 9);

    this->heartbeat();

    return true;
}

void DS2::serviceInputs(unsigned char serviceb) {

    serviceb = serviceb & 0x80;

    if (serviceb == 0) {
        return;
    }

    delay(50);

    unsigned char service[] = { DS2_SERVICE_INPUTS, (unsigned char) (serviceb & 0x80) }; // TODO check if 0b10000000 actually works
    Serial.write(service, sizeof(service));

    delay(50);
}

void DS2::playerInputs(unsigned char p1b1, unsigned char p1b2, unsigned char p2b1, unsigned char p2b2) {

    // Deathsmiles 2 player input packets match JVS packets exactly, without the framing and control bytes.
    // Instead a different prefix byte is used, similar to other DS2 commands

    // Format byte2
    // bit7 = button 3
    // ... Nothing relevant
    // bit1 = coin. TODO confirm?
    // we will binary AND byte2 with 0x80 (0b10000000) to clean up traces of buttons 4, 5, 6...

    // TODO is there a packet for bulk P1+P2 controls?

    // clean up opposite directions in the current packet
    if ((p1b1 & DS2_INPUT_UP) && (p1b1 & DS2_INPUT_DOWN)) {
        p1b1 &= ~(DS2_INPUT_UP | DS2_INPUT_DOWN);
    }
    if ((p1b1 & DS2_INPUT_LEFT) && (p1b1 & DS2_INPUT_RIGHT)) {
        p1b1 &= ~(DS2_INPUT_LEFT|DS2_INPUT_RIGHT);
    }

    // aggregate current input packet in the buffered state
    this->_buffer_p1b1 |= p1b1;
    this->_buffer_p1b2 |= p1b2;

    // clean up opposing directions in the buffer
    // latest (current packet) direction wins
    if (p1b1 & DS2_INPUT_UP) {
        this->_buffer_p1b1 &= ~DS2_INPUT_DOWN;
    }
    if (p1b1 & DS2_INPUT_DOWN) {
        this->_buffer_p1b1 &= ~DS2_INPUT_UP;
    }
    if (p1b1 & DS2_INPUT_LEFT) {
        this->_buffer_p1b1 &= ~DS2_INPUT_RIGHT;
    }
    if (p1b1 & DS2_INPUT_RIGHT) {
        this->_buffer_p1b1 &= ~DS2_INPUT_LEFT;
    }

    if (DS2_ENABLE_P2) {
        if ((p2b1 & DS2_INPUT_UP) && (p2b1 & DS2_INPUT_DOWN)) {
            p2b1 &= ~(DS2_INPUT_UP | DS2_INPUT_DOWN);
        }
        if ((p2b1 & DS2_INPUT_LEFT) && (p2b1 & DS2_INPUT_RIGHT)) {
            p2b1 &= ~(DS2_INPUT_LEFT|DS2_INPUT_RIGHT);
        }

        this->_buffer_p2b1 |= p2b1;
        this->_buffer_p2b2 |= p2b2;

        if (p2b1 & DS2_INPUT_UP) {
            this->_buffer_p2b1 &= ~DS2_INPUT_DOWN;
        }
        if (p2b1 & DS2_INPUT_DOWN) {
            this->_buffer_p2b1 &= ~DS2_INPUT_UP;
        }
        if (p2b1 & DS2_INPUT_LEFT) {
            this->_buffer_p2b1 &= ~DS2_INPUT_RIGHT;
        }
        if (p2b1 & DS2_INPUT_RIGHT) {
            this->_buffer_p2b1 &= ~DS2_INPUT_LEFT;
        }
    }

    unsigned long now = millis();

    // The game really hates it when you send too many commands per frame.
    // They may be ignored and cause dropped inputs and very sluggish controls.
    // Also they easily trigger an error screen (error 8000).
    // As such, we need a strategy to reduce the number of commands sent while avoiding dropped inputs.
    //
    // The strategy is:
    // - poll JVS often (the process takes roughly 3~4ms, so we clock it every 5ms for consistency (MAIN_LOOP_DELAY)
    // - aggregate all the inputs in a buffer
    // - perform opposite direction cleaning (the game also really dislikes it)
    // - every frame (DS2_INPUT_DELAY), send an update of the buffered state to the game
    // 
    // This can still trigger some errors due to the number of cmds, so we go further:
    // - keep the last state sent in memory, and avoid sending duplicates as long as the JVS polling
    //   returns the same state
    // 
    // But in testing, this produced weird glitches on very quick joystick move sequences e.g. half circle,
    // where one input would remain stuck for some reason (and we wouldnt send a corrective follow up because
    // of our optimization).
    // As such, we reduce the optimization by only avoiding duplicates after the 2nd one,
    // giving ample time to the game to process everything.
    // This produces the best results I've found so far, but maybe there's a better way?
    //
    // If you're reading this, sorry for the mouthful. It was annoying to test.
    // TODO how does the original board solve this? I would expect something similar at least

    if (now >= this->_p1Deadline) {
        // prevent sending duplicates after the 2nd time
        // we still send the duplicate the first time as a safety net
        if (! (this->lastp1b1 == this->prevlastp1b1 && this->lastp1b1 == this->_buffer_p1b1
                    && this->lastp1b2 == this->prevlastp1b2 && this->lastp1b2 == this->_buffer_p1b2)) {

            unsigned char p1[] = { DS2_P1_INPUTS, this->_buffer_p1b1, (unsigned char) (this->_buffer_p1b2 & 0x80) };
            Serial.write(p1, sizeof(p1));

            // send asap
            // the game doesnt like to receive P1 *and* P2 inputs at the same time
            Serial.send_now();

            this->prevlastp1b1 = this->lastp1b1;
            this->prevlastp1b2 = this->lastp1b2;
            this->lastp1b1 = this->_buffer_p1b1;
            this->lastp1b2 = this->_buffer_p1b2;
        }
        this->_p1Deadline = now + DS2_INPUT_DELAY;
        this->_buffer_p1b1 = 0;
        this->_buffer_p1b2 = 0;
    }

    if (DS2_ENABLE_P2) {
        if (now >= this->_p2Deadline) {
            // prevent sending duplicates after the 2nd time
            // we still send the duplicate the first time as a safety net
            if (! (this->lastp2b1 == this->prevlastp2b1 && this->lastp2b1 == this->_buffer_p2b1
                        && this->lastp2b2 == this->prevlastp2b2 && this->lastp2b2 == this->_buffer_p2b2)) {

                unsigned char p2[] = { DS2_P2_INPUTS, this->_buffer_p2b1, (unsigned char) (this->_buffer_p2b2 & 0x80) };
                Serial.write(p2, sizeof(p2));

                // send asap
                // the game doesnt like to receive P1 *and* P2 inputs at the same time
                Serial.send_now();

                this->prevlastp2b1 = this->lastp2b1;
                this->prevlastp2b2 = this->lastp2b2;
                this->lastp2b1 = this->_buffer_p2b1;
                this->lastp2b2 = this->_buffer_p2b2;
            }
            this->_p2Deadline = now + DS2_INPUT_DELAY;
            this->_buffer_p2b1 = 0;
            this->_buffer_p2b2 = 0;
        }
    }
}

bool DS2::coinInputs(unsigned char c1b1, unsigned char c1b2, unsigned char c2b1, unsigned char c2b2) {

    // recompose 14bits coin count omitting the 2 bits used by coin status

    unsigned short newCoins1 = (((unsigned short) c1b1 & 0b00111111) << 8) + c1b2;
    unsigned short newCoins2 = (((unsigned short) c2b1 & 0b00111111) << 8) + c2b2;

    int extraCoins = 0;

    if (newCoins1 > this->coins1) {
        // added coin1
        extraCoins += newCoins1 - this->coins1;
        this->coins1 = newCoins1;
    }

    if (newCoins2 > this->coins2) {
        // added coin2
        extraCoins += newCoins2 - this->coins2;
        this->coins2 = newCoins2;
    }

    if (extraCoins == 0) {
        return false;
    }

    // we need delay here to avoid triggering error screens in the game.

    delay(DS2_DELAY_COINS);

    for (int i = 0; i < extraCoins; i++) {
        Serial.write(DS2_COIN_INPUTS); // TODO check if a parameter can be passed instead of looping
        delay(DS2_DELAY_COINS);
    }

    delay(DS2_DELAY_COINS);

    return true;
}
