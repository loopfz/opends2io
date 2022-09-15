#ifndef __JVSDECODER_H__
#define __JVSDECODER_H__

#include "Arduino.h"
#include "rgbled.h"
#include "ds2.h"
#include "decoder.h"

class JVSDecoder : public Decoder {
public:
	JVSDecoder(DS2&, RGBLed&);
	void init();
	void process();
    void reset();
    bool needReset();

private:
	HardwareSerial& _JVSSerial;
    DS2 _ds2;
    RGBLed _errLed;

	bool initialized;

    int _numPlayers;
    int _numCoins;

    int readNextByte();
    int readNextUnescapedByte();

	void resetSlaves();
	int* cmd(char destination, char data[], int size);
    void write_packet(int destination, unsigned char data[],int size);
    bool initSlave(int board);
    int* cmd(int destination, unsigned char data[], int size,
            unsigned char r1, unsigned char g1, unsigned char b1,
            unsigned char r2, unsigned char g2, unsigned char b2,
            int &resLength);
    bool handleStatusByte(unsigned char status,
            unsigned char r1, unsigned char g1, unsigned char b1,
            unsigned char r2, unsigned char g2, unsigned char b2,
            unsigned int durationMs);
    bool handleReportByte(unsigned char report,
            unsigned char r1, unsigned char g1, unsigned char b1,
            unsigned char r2, unsigned char g2, unsigned char b2,
            unsigned int durationMs);
    bool timeout(int &counter, unsigned long deadline);
};

#endif /* __JVSDECODER_H__ */
