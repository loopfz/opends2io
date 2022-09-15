#include "Arduino.h"
#include "rgbled.h"

RGBLed::RGBLed(int redp, int greenp, int bluep) {

   this->init(redp, greenp, bluep, DEFAULT_DURATION_MS);
}

RGBLed::RGBLed(int redp, int greenp, int bluep, int durationMs) {

   this->init(redp, greenp, bluep, durationMs);
}

void RGBLed::init(unsigned char redp, unsigned char greenp, unsigned char bluep, unsigned int durationMs) {

    pinMode(redp, OUTPUT);
    pinMode(greenp, OUTPUT);
    pinMode(bluep, OUTPUT);
    this->_redp = redp;
    this->_greenp = greenp;
    this->_bluep = bluep;
    this->_defaultDuration = durationMs;
    this->reset();
}

void RGBLed::reset() {

    this->setLed(0, 0, 0);
    this->_timeToStop = 0;
    this->_blink = false;
    this->_blinkFreqMs = 0;
    this->_nextBlink = 0;
    this->_blinkPhase = false;
    this->_red1 = 0;
    this->_green1 = 0;
    this->_blue1 = 0;
    this->_red2 = 0;
    this->_green2 = 0;
    this->_blue2 = 0;
}

void RGBLed::setLed(unsigned char r, unsigned char g, unsigned char b) {

    analogWrite(this->_redp, r);
    analogWrite(this->_greenp, g);
    analogWrite(this->_bluep, b);
}

void RGBLed::SolidLed(unsigned char r, unsigned char g, unsigned char b, unsigned int durationMs) {

    if (this->tooSoon()) {
        return;
    }

    unsigned long now = millis();
    this->_timeToStop = now + durationMs;
    this->_red1 = r;
    this->_green1 = g;
    this->_blue1 = b;
    this->setLed(r, g, b);
}

void RGBLed::SolidLed(unsigned char r, unsigned char g, unsigned char b) {

    this->SolidLed(r, g, b, this->_defaultDuration);
}

void RGBLed::SlowBlinkLed(unsigned char r, unsigned char g, unsigned char b,
                          unsigned char r2, unsigned char g2, unsigned char b2,
                          unsigned int durationMs) {
    this->genericBlinkLed(r, g, b, r2, g2, b2,  durationMs, SLOW_BLINK_FREQ_MS);
}

void RGBLed::SlowBlinkLed(unsigned char r, unsigned char g, unsigned char b,
                          unsigned char r2, unsigned char g2, unsigned char b2) {
    this->SlowBlinkLed(r, g, b, r2, g2, b2, DEFAULT_DURATION_MS);
}

void RGBLed::BlinkLed(unsigned char r, unsigned char g, unsigned char b,
                      unsigned char r2, unsigned char g2, unsigned char b2,
                      unsigned int durationMs) {

    this->genericBlinkLed(r, g, b, r2, g2, b2, durationMs, BLINK_FREQ_MS);
}

void RGBLed::BlinkLed(unsigned char r, unsigned char g, unsigned char b,
                      unsigned char r2, unsigned char g2, unsigned char b2) {

    this->BlinkLed(r, g, b, r2, g2, b2, DEFAULT_DURATION_MS);
}

void RGBLed::FastBlinkLed(unsigned char r, unsigned char g, unsigned char b,
                          unsigned char r2, unsigned char g2, unsigned char b2,
                          unsigned int durationMs) {

    this->genericBlinkLed(r, g, b, r2, g2, b2, durationMs, FAST_BLINK_FREQ_MS);
}

void RGBLed::FastBlinkLed(unsigned char r, unsigned char g, unsigned char b,
                          unsigned char r2, unsigned char g2, unsigned char b2) {

    this->FastBlinkLed(r, g, b, r2, g2, b2, DEFAULT_DURATION_MS);
}

void RGBLed::genericBlinkLed(unsigned char r, unsigned char g, unsigned char b,
                             unsigned char r2, unsigned char g2, unsigned char b2,
                             unsigned int durationMs, unsigned int blinkFreqMs) {
    
    this->SolidLed(r, g, b, durationMs);
    this->_blink = true; 
    this->_red2 = r2;
    this->_green2 = g2;
    this->_blue2 = b2;
    this->_blinkFreqMs = blinkFreqMs;
    this->_nextBlink = millis() + this->_blinkFreqMs;
    this->_blinkPhase = true;
}

void RGBLed::Tick() {

    if (this->_timeToStop == 0) {
        return; 
    }

    unsigned long now = millis();

    if (this->_timeToStop < now) {
        this->reset();
        return;
    }

    if (this->_blink && this->_nextBlink < now) {
        if (this->_blinkPhase) {
            this->blink(now, this->_red2, this->_green2, this->_blue2);
        } else {
            this->blink(now, this->_red1, this->_green1, this->_blue1);
        }
    }
}

void RGBLed::blink(unsigned long now, unsigned char r, unsigned char g, unsigned char b) {
    this->setLed(r, g, b);
    this->_nextBlink = now + this->_blinkFreqMs;
    this->_blinkPhase = !this->_blinkPhase;
}

bool RGBLed::tooSoon() {

    return (millis() < this->_timeToStop);
}
