#ifndef __DS2_H__
#define __DS2_H__

#include "rgbled.h"

class DS2 {
    public:
        DS2(RGBLed& errLed);
        bool init();
        void heartbeat();

        void serviceInputs(unsigned char serviceb);
        void playerInputs(unsigned char p1b1, unsigned char p1b2, unsigned char p2b1, unsigned char p2b2);
        bool coinInputs(unsigned char c1b1, unsigned char c1b2, unsigned char c2b1, unsigned char c2b2);

    private:

        unsigned long _p1Deadline;
        unsigned long _p2Deadline;

        unsigned char lastp1b1, lastp1b2, lastp2b1, lastp2b2;
        unsigned char prevlastp1b1, prevlastp1b2, prevlastp2b1, prevlastp2b2;
        unsigned char _buffer_p1b1, _buffer_p1b2, _buffer_p2b1, _buffer_p2b2;

        RGBLed _errLed;
        unsigned long lastHeartbeat;

        unsigned short coins1;
        unsigned short coins2;

};

#endif /* __DS2_H__ */
