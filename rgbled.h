#ifndef __RGBLED_H__
#define __RGBLED_H__

#define DEFAULT_DURATION_MS 10000
#define SLOW_BLINK_FREQ_MS 3333
#define BLINK_FREQ_MS 1000
#define FAST_BLINK_FREQ_MS 333

class RGBLed {
    public:

        RGBLed(int redp, int greenp, int bluep);
        RGBLed(int redp, int greenp, int bluep, int durationMs);

        void SolidLed(unsigned char r, unsigned char g, unsigned char b);
        void SolidLed(unsigned char r, unsigned char g, unsigned char b, unsigned int durationMs);

        // These blink prototypes look ugly but they're convenient to use with the LED_xxx defines

        void SlowBlinkLed(unsigned char r, unsigned char g, unsigned char b,
                          unsigned char r2, unsigned char g2, unsigned char b2);
        void SlowBlinkLed(unsigned char r, unsigned char g, unsigned char b,
                          unsigned char r2, unsigned char g2, unsigned char b2,
                          unsigned int durationMs);

        void BlinkLed(unsigned char r, unsigned char g, unsigned char b,
                      unsigned char r2, unsigned char g2, unsigned char b2);
        void BlinkLed(unsigned char r, unsigned char g, unsigned char b,
                      unsigned char r2, unsigned char g2, unsigned char b2,
                      unsigned int durationMs);

        void FastBlinkLed(unsigned char r, unsigned char g, unsigned char b,
                          unsigned char r2, unsigned char g2, unsigned char b2);
        void FastBlinkLed(unsigned char r, unsigned char g, unsigned char b,
                          unsigned char r2, unsigned char g2, unsigned char b2,
                          unsigned int durationMs);

        void Tick();

    private:
        int _redp, _greenp, _bluep;
        int _defaultDuration;

        unsigned long _timeToStop;

        bool _blink;
        unsigned int _blinkFreqMs;
        unsigned long _nextBlink;
        bool _blinkPhase;
        // first color for blink
        unsigned char _red1;
        unsigned char _green1;
        unsigned char _blue1;
        // second color for blink
        unsigned char _red2;
        unsigned char _green2;
        unsigned char _blue2;

        bool tooSoon();
        void init(unsigned char redp, unsigned char bluep, unsigned char greenp, unsigned int durationMs);
        void reset();
        void setLed(unsigned char r, unsigned char g, unsigned char b);
        void blink(unsigned long now, unsigned char r, unsigned char g, unsigned char b);
        void genericBlinkLed(unsigned char r, unsigned char g, unsigned char b, unsigned char r2, unsigned char g2, unsigned char b2, unsigned int durationMs, unsigned int blinkFreqMs);
};

#endif /* __RGBLED_H__ */
