#ifndef __DB15DECODER_H__
#define __DB15DECODER_H__

#include "Arduino.h"
#include "rgbled.h"
#include "ds2.h"
#include "decoder.h"

class DB15Decoder : public Decoder {
public:
	DB15Decoder(DS2&, RGBLed&);
	void init();
	void process();
    void reset();
    bool needReset();

private:
    DS2 _ds2;
    RGBLed _errLed;
};

#endif /* __DB15DECODER_H__ */
