#include "Arduino.h"
#include "JVSDecoder.h"
#include "constants.h"
#include "ds2.h"

JVSDecoder::JVSDecoder(DS2& ds2, RGBLed& errLed) :
_JVSSerial(Serial1), _ds2(ds2), _errLed(errLed)
{
	this->initialized = false;
    this->_numPlayers = 0;
    this->_numCoins = 0;
}

void JVSDecoder::resetSlaves() {

    delay(JVS_DELAY_MASTER_START);

    if (!JVS_DEBUG) {
        this->_ds2.heartbeat();
    }

    // resets the communication status of all slaves
    // fits on 2 bytes on purpose (with a fixed/constant 2nd byte) to avoid sending it by accident
    unsigned char str[] = { CMD_RESET, CMD_RESET_ARG };

    if (JVS_DEBUG) {
        Serial.println("### JVS reset ###");
    }

    // destination BROADCAST to reach all slaves
	this->write_packet(BROADCAST, str, 2);

    delay(JVS_DELAY_SLAVE_START);

    if (!JVS_DEBUG) {
        this->_ds2.heartbeat();
    }

    // JVS spec recommends repeating the reset cmd 2x
	this->write_packet(BROADCAST, str, 2);

    delay(JVS_DELAY_SLAVE_START);

    if (!JVS_DEBUG) {
        this->_ds2.heartbeat();
    }

    this->_numPlayers = 0;
    this->_numCoins = 0;
}

void blinkN(int n, int ms) {
  for (int i = 0; i < n; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(ms);
  }
}

void JVSDecoder::init() {

    // JVS, coming from rs485 transceiver e.g. SN65176BP
    this->_JVSSerial.begin(115200, DE_PIN);

    // set sense line high
    pinMode(SENSE_PIN, OUTPUT);
    analogWrite(SENSE_PIN,1023);

}

void JVSDecoder::reset() {

    digitalWrite(LED_BUILTIN, HIGH);

    // this HIGH is caused by our previous analogWrite, the cable is not connected
    while (analogRead(SENSE_PIN) > 900) {
        delay(1000);
        if (JVS_DEBUG) {
            Serial.println("waiting for jvs cable to be connected");
        } else {
            this->_ds2.heartbeat();
        }
    }

    if (JVS_DEBUG) {
        Serial.println("jvs cable connected");
    }

    this->initialized = false;

    // initialize all JVS slaves
    while (!this->initialized){

        if (!JVS_DEBUG) {
            this->_ds2.heartbeat();
        }
      
        // blink off momentarily to show retry
        digitalWrite(LED_BUILTIN, LOW);
        this->resetSlaves();
        digitalWrite(LED_BUILTIN, HIGH);

        if (!JVS_DEBUG) {
            this->_ds2.heartbeat();
        }
      
        int i = 1; // increment JVS slave ids, starting with 1 (we are 0)

        // the slaves will set the SENSE line high (> 2.5v) while initializing
        // it goes back low once finished
        // analogRead returns 0 == 0v to 1023 == 5v, we want to test for roughly >2v
        while (JVS_IGNORE_SENSE || analogRead(SENSE_PIN) >= JVS_SENSE_LOW){
            if (JVS_DEBUG) {
                Serial.print("looking for slaves, next = ");
                Serial.println(i);
            }
            // init the next slave
            if (!this->initSlave(i++)) {
                if (JVS_DEBUG) {
                    Serial.println("slave init failed, reseting");
                }
                this->initialized = false;
                break;
            }
            if (JVS_IGNORE_SENSE) {
                break;
            }
            if (!JVS_DEBUG) {
                this->_ds2.heartbeat();
            }
        }

        if (!JVS_DEBUG) {
            this->_ds2.heartbeat();
        }
    }

    digitalWrite(LED_BUILTIN, LOW);
}

bool JVSDecoder::needReset() {
    return (analogRead(SENSE_PIN) > JVS_SENSE_LOW); // cable disconnected
}

bool JVSDecoder::initSlave(int board) {

    // Each step in the init routine will be checked for success.
    // If it fails, it will interrupt the process and the error led will report
    // a specific error code.

    int resLength = 0;

    if (JVS_DEBUG) {
        Serial.println("-- sending SETADDR");
    }

    // Assign address to slave. It will be forwarded along the JVS daisy chain
    // as long as there is a SENSE line reading high further in the chain.
    // As such, it will always be attributed to the furthest non-initialized slave.
	unsigned char str[] = { CMD_SETADDR, (unsigned char)board };
	int *res = this->cmd(BROADCAST, str, 2, LED_BLUE, LED_GREEN, resLength);
    if (res == NULL) {
        return false;
    } else {
        // The response only contains the status/report, nothing further to check.
        free(res);
    }

    delay(JVS_DELAY_SETADDR);

    // SETADDR used BROADCAST (0xFF) as destination, but from now on we target only
    // the newly assigned slave id (int board).

    if (JVS_DEBUG) {
        Serial.println("-- sending IOIDENT");
    }

    // Request slave device id
    // It is an ascii null-terminated string of maximum 100 chars
    // Contains: Maker name, I/O board code, software version number, and details
    // Example: "NAMCO LTD.;I/O PCB-1000;ver1.0;for domestic only,no analog input"
	unsigned char str1[] = { CMD_IOIDENT };
	res = this->cmd(board, str1, 1, LED_BLUE, LED_PINK, resLength);
    if (res == NULL) {
        return false;
    } else {
        // we skip the response once the status/report is checked since we don't care that much
        free(res);
    }

    if (JVS_DEBUG) {
        Serial.println("-- sending CMDREV");
    }

    // Request the revision of command format supported by the slave
    // Expected: rev1.3, i.e. "13"
	unsigned char str2[] = { CMD_CMDREV };
	res = this->cmd(board, str2, 1, LED_BLUE, LED_WHITE, resLength);
    if (res == NULL) {
        return false;
    } else {
        // we skip the response once the status/report is checked since we don't care that much
        free(res);
    }

    if (JVS_DEBUG) {
        Serial.println("-- sending JVSREV");
    }

    // Request the revision of JVS supported by the slave
    // Expected: JVS REV3.0 i.e. "30"
	unsigned char str3[] = { CMD_JVSREV };
	res = this->cmd(board, str3, 1, LED_BLUE, LED_DARK_BLUE, resLength);
    if (res == NULL) {
        return false;
    } else {
        // we skip the response once the status/report is checked since we don't care that much
        free(res);
    }

    if (JVS_DEBUG) {
        Serial.println("-- sending COMMVER");
    }

    // Request the revision of the communication system supported by the slave
    // Expected: REV1.0 i.e. "10"
	unsigned char str4[] = { CMD_COMMVER };
	res = this->cmd(board, str4, 1, LED_BLUE, LED_OFF, resLength);
    if (res == NULL) {
        return false;
    } else {
        // we skip the response once the status/report is checked since we don't care that much
        free(res);
    }

    if (JVS_DEBUG) {
        Serial.println("-- sending FEATCHECK");
    }

    // Request the functions supported by the slave
    // examples: digital switch input, coin input, analog input, rotary input, ...
	unsigned char str5[] = { CMD_FEATCHECK };
	res = this->cmd(board, str5, 1, LED_GREEN, LED_OFF, resLength);
    if (res == NULL) {
        return false;
    }

    int numPlayers = 0;
    int numButtons = 0;
    int numCoins = 0;

    // only check capacity for JVS slave 1 (furthest in the JVS daisy chain)
    // we will address all control/coins requests later to id 1
    if (board == 1) {  
        for (int i = 2; i < resLength - 1; i += 4) {
            if (i+4 > resLength - 1) {
                // too short for current feature + 3 params
                break;
            }
            switch (res[i]) {
                case 1:
                    // switch input
                    numPlayers = res[i+1];
                    if (numPlayers > 2) {
                        numPlayers = 2;
                    }
                    numButtons = res[i+2];
                case 2:
                    // coin input
                    numCoins = res[i+1];
                    if (numCoins > 2) {
                        numCoins = 2;
                    }
            }
        }
    }

    free(res);

    if (board == 1) {
        if (numPlayers == 0) {
            if (JVS_DEBUG) {
                Serial.println("0 players supported");
            }
            this->_errLed.SlowBlinkLed(LED_GREEN, LED_DARK_GREEN, 60000);
            if (!JVS_PERMISSIVE) {
                return false;
            }
            numPlayers = 2; // default to 2 in permissive mode
        }
        if (numButtons < 9) {
            // minimum 9 switches: 4 directions + 3 buttons + start + service
            if (JVS_DEBUG) {
                Serial.print("not enough buttons: ");
                Serial.println(numButtons);
            }
            this->_errLed.BlinkLed(LED_GREEN, LED_DARK_GREEN, 60000);
            if (!JVS_PERMISSIVE) {
                return false;
            }
        }
        if (numCoins == 0) {
            if (JVS_DEBUG) {
                Serial.println("0 coins supported");
            }
            this->_errLed.SlowBlinkLed(LED_GREEN, LED_DARK_GREEN, 60000);
            if (!JVS_PERMISSIVE) {
                return false;
            }
            numCoins = 2; // default to 2 in permissive mode
        }
        this->_numPlayers = numPlayers;
        this->_numCoins = numCoins;
    }

    this->initialized = true;

    return true;
}

int JVSDecoder::readNextByte() {
    while (!this->_JVSSerial.available()) {
    }
    return this->_JVSSerial.read();
}

int JVSDecoder::readNextUnescapedByte() {
    int b = this->readNextByte();
    if (b == MARKBYTE) {
        return this->readNextByte() + 1;
    }
    return b; 
}

void JVSDecoder::process() {

    // read input for slave id 1 and process it
    // slave id 1 is the furthest in the JVS daisy chain
    int board = 1;

    // combine 2 requests:
    // - read digital switch inputs
    //   args: number of players, number of resp bytes per player
    // - read coin inputs
    //   args: slot count
	unsigned char str[] = { CMD_SWINP, (unsigned char) this->_numPlayers, 2,
                            CMD_COININP, (unsigned char) this->_numCoins};

	this->write_packet(board, str, sizeof(str));

    // loop until we find a response addressed to us
    while (1) {
        while (this->readNextByte() != SYNC) {
        } // wait for sync framing the response
        if (this->readNextUnescapedByte() != 0) { // destination id
            // this result is addressed to another id (we are master / id 0)
            // start over
            continue;
        }
        break;
    }

	int length = this->readNextUnescapedByte();

    // Expected response bytes:
    // 00 - STATUS
    // 01 - SWINP cmd report
    // 02 - TEST switches
    //      bit7: TEST sw
    //      bit6: TILT1
    //      bit5: TILT2
    //      bit4: TILT3
    //      bit3, bit2, bit1, bit0: n/a
    // 03 - p1 switches (1/2)
    //      bit7: start
    //      bit6: service
    //      bit5: up
    //      bit4: down
    //      bit3: left
    //      bit2: right
    //      bit1: b1
    //      bit0: b2
    // 04 - p1 switches (2/2)
    //      bit7: b3
    //      bit6: b4
    //      bit5: b5
    //      bit4: b6
    //      bit3: b7
    //      bit2: b8
    //      bit1, bit0: n/a
    // 05 - p2 switches (1/2) [optional]
    //      bit7: start
    //      bit6: service
    //      bit5: up
    //      bit4: down
    //      bit3: left
    //      bit2: right
    //      bit1: b1
    //      bit0: b2
    // 06 - p2 switches (2/2) [optional]
    //      bit7: b3
    //      bit6: b4
    //      bit5: b5
    //      bit4: b6
    //      bit3: b7
    //      bit2: b8
    //      bit1, bit0: n/a
    // 07 - COININP cmd report
    // 08 - slot1 (1/2)
    //      bit7, bit6 = coin status
    //      bit5, bit4, bit3, bit2, bit1 = most significant bits of coin count
    // 09 - slot1 (2/2)
    //      next 8 least significant bits of coin count
    // 10 - slot2 (1/2) [optional]
    //      bit7, bit6 = coin status
    //      bit5, bit4, bit3, bit2, bit1 = most significant bits of coin count
    // 11 - slot2 (2/2) [optional]
    //      next 8 least significant bits of coin count
    // 12 - SUM

    unsigned char buf[13] = {0};
    int byteCounter = 0;

    // check buf len before processing
    int expectedLen = 5 + (this->_numPlayers * 2) + (this->_numCoins * 2); // from 9 to 13
    if (length != expectedLen) {
        blinkN(3, 100);
        this->_errLed.SlowBlinkLed(LED_RED, LED_OFF, 60000);
        return;
    }

	while (byteCounter < length) {
		int incomingByte = this->readNextUnescapedByte();
        buf[byteCounter++] = (unsigned char) incomingByte;
    }

    unsigned int sum = length;
    for (int i = 0; i < length-1 /* -1 to skip last byte which is sum */; i++) {
        sum = (sum + buf[i]) % 256;
    }

    if (sum != (unsigned int) buf[length-1]) {
        // sum is wrong
        this->_errLed.SlowBlinkLed(LED_RED, LED_OFF, 60000);
        if (!JVS_PERMISSIVE) {
            blinkN(4, 100);
            return;
        }
    }

    unsigned char   status = buf[0],
                    SWINP_report = buf[1],
                    service = buf[2],
                    p1b1 = buf[3],
                    p1b2 = buf[4],
                    p2b1 = 0, p2b2 = 0;

    int COININP_idx = 5;
    if (this->_numPlayers == 2) {
        COININP_idx += 2;
        p2b1 = buf[5];
        p2b2 = buf[6];
    }

    unsigned char   COININP_report = buf[COININP_idx],
                    c1b1 = buf[COININP_idx+1],
                    c1b2 = buf[COININP_idx+2],
                    c2b1 = 0, c2b2 = 0;

    if (this->_numCoins == 2) {
        c2b1 = buf[COININP_idx+3];
        c2b2 = buf[COININP_idx+4];
    }

    this->handleStatusByte(status, LED_RED, LED_OFF, 60000);

    this->handleReportByte(SWINP_report, LED_RED, LED_OFF, 60000);

    this->handleReportByte(COININP_report, LED_RED, LED_DARK_RED, 60000);

    // COIN statuses (stored on bit7 and bit6):
    // 0 = normal
    // 1 = coin jam
    // 2 = counter is disconnected
    // 3 = busy (for card reading systems, which are mixed with coin slots)

    if ((c1b1 >> 6) != 0) {
        // bad coin1 status
        int stat = c1b1 >> 6;
        switch (stat) {
            case 1:
            case 3:
            _errLed.FastBlinkLed(LED_RED, LED_DARK_RED, 10000);
            break;
        }
    }

    if (this->_numCoins == 2) {
        if ((c2b1 >> 6) != 0) {
            // bad coin2 status
            int stat = c2b1 >> 6;
            switch (stat) {
                case 1:
                case 3:
                    _errLed.FastBlinkLed(LED_RED, LED_DARK_RED, 10000);
                    break;
            }
        }
    }

    if (!JVS_DEBUG) {
        if (!this->_ds2.coinInputs(c1b1, c1b2, c2b1, c2b2)) {
            this->_ds2.serviceInputs(service);
            this->_ds2.playerInputs(p1b1, p1b2, p2b1, p2b2);
        }

        this->_ds2.heartbeat();
    } else {
        Serial.println("--- JVS controls");
        Serial.print("service: ");
        Serial.println(service, BIN);
        Serial.print("p1b1: ");
        Serial.println(p1b1, BIN);
        Serial.print("p1b2: ");
        Serial.println(p1b2, BIN);
        Serial.print("p2b1: ");
        Serial.println(p2b1, BIN);
        Serial.print("p2b2: ");
        Serial.println(p2b2, BIN);
        Serial.print("c1b1: ");
        Serial.println(c1b1, BIN);
        Serial.print("c1b2: ");
        Serial.println(c1b2, BIN);
        Serial.print("c2b1: ");
        Serial.println(c2b1, BIN);
        Serial.print("c2b2: ");
        Serial.println(c2b2, BIN);
        Serial.send_now();
    }
}

bool JVSDecoder::handleStatusByte(unsigned char status,
        unsigned char r1, unsigned char g1, unsigned char b1,
        unsigned char r2, unsigned char g2, unsigned char b2,
        unsigned int durationMs) {

    // STATUS byte values
    // 1 = normal
    // 2 = unknown command
    // 3 = checksum error
    // 4 = overflow (too much data in response)

    switch (status) {
        case 1:
            // good !
            if (JVS_DEBUG) {
                Serial.println("status is good");
            }
            break;
        case 2:
            if (JVS_DEBUG) {
                Serial.println("status: unknown command");
            }
            _errLed.SlowBlinkLed(r1, g1, b1, r2, g2, b2, durationMs);
            return false;
        case 3:
            if (JVS_DEBUG) {
                Serial.println("status: checksum error");
            }
            _errLed.SlowBlinkLed(r1, g1, b1, r2, g2, b2, durationMs);
            return false;
        case 4:
            if (JVS_DEBUG) {
                Serial.println("status: overflow");
            }
            _errLed.SlowBlinkLed(r1, g1, b1, r2, g2, b2, durationMs);
            return false;
        default:
            if (JVS_DEBUG) {
                Serial.print("status: unknown status value: ");
                Serial.println(status);
            }
            // nonsense value, treat it similarly as bad sum
            _errLed.SlowBlinkLed(r1, g1, b1, r2, g2, b2, durationMs);
            return false;
    }

    return true;
}

bool JVSDecoder::handleReportByte(unsigned char report,
        unsigned char r1, unsigned char g1, unsigned char b1,
        unsigned char r2, unsigned char g2, unsigned char b2,
        unsigned int durationMs) {

    // Report byte values
    // 1 = normal
    // 2 = parameter error, incorrect number of params
    // 3 = parameter error, invalid data supplied
    // 4 = busy

    switch (report) {
        case 1:
            // good !
            if (JVS_DEBUG) {
                Serial.println("report is good");
            }
            break;
        case 2:
            if (JVS_DEBUG) {
                Serial.println("report: param error, incorrect number of params");
            }
            _errLed.BlinkLed(r1, g1, b1, r2, g2, b2, durationMs);
            return false;
        case 3:
            if (JVS_DEBUG) {
                Serial.println("report: param error, invalid data supplied");
            }
            _errLed.BlinkLed(r1, g1, b1, r2, g2, b2, durationMs);
            return false;
        case 4:
            if (JVS_DEBUG) {
                Serial.println("report: busy");
            }
            _errLed.BlinkLed(r1, g1, b1, r2, g2, b2, durationMs);
            return false;
    }

    return true;
}

// Helper function used during init.
// Writes packet then aggregates the response in a buffer and returns it
// then sleeps some amount of time.
// Not used during the main input check loop for timing reasons.
// The returned buffer is malloc'ed and it is the caller's responsibility to free it.
int* JVSDecoder::cmd(int destination, unsigned char data[], int size,
        unsigned char r1, unsigned char g1, unsigned char b1,
        unsigned char r2, unsigned char g2, unsigned char b2,
        int &resLength) {

	this->write_packet(destination, data, size);

	int incomingByte;

    resLength = 0;

    unsigned long deadline = millis() + 2000; // 10sec deadline. Keep short for snappy retries.
    int waitCount = 0;

    // loop until we find a response addressed to us
    // timeout while waiting for SYNC+target to avoid blocking forever
    // if the slave was powered up after us and missed the reset cmd
    while (1) {
        // read byte logic is inlined to implement timeout
        while (1) {
            if (_JVSSerial.available()) {
                int incoming = this->_JVSSerial.read();
                if (incoming == SYNC) {
                    if (JVS_DEBUG) {
                        Serial.println("found SYNC");
                    }
                    break;
                } else {
                    if (JVS_DEBUG) {
                        Serial.print("found non-sync byte: ");
                        Serial.println(incoming, HEX);
                    }
                }
            }
            if (this->timeout(waitCount, deadline)) {
                if (JVS_DEBUG) {
                    Serial.println("timeout");
                }
                this->_errLed.SolidLed(r1, g1, b1, 10000);
                return NULL;
            }
        }
        while (!_JVSSerial.available()) {
            if (this->timeout(waitCount, deadline)) {
                if (JVS_DEBUG) {
                    Serial.println("timeout");
                }
                this->_errLed.SolidLed(r1, g1, b1, 10000);
                return NULL;
            }
        }
        int target = this->_JVSSerial.read();
        if (target != 0) { // destination id
            // this result is addressed to another id (we are master / id 0)
            // start over
            if (JVS_DEBUG) {
                Serial.print("message not addressed to us: ");
                Serial.println(target, HEX);
            }
            continue;
        }
        break;
    }

	int length = this->readNextUnescapedByte();

    if (JVS_DEBUG) {
        Serial.print("length is ");
        Serial.println(length);
    }

	int counter = 0;
	int* res = (int*) malloc(length * sizeof(int));

    // read the rest of the response and aggregate it in the res buffer
	while (counter < length) {
		incomingByte = this->readNextUnescapedByte();
		res[counter] = incomingByte;
		counter++;
	}

    if (JVS_DEBUG) {
        Serial.println("done reading response bytes");
    }

    unsigned int sum = length;
    for (int i = 0; i < length - 1 /* -1 to skip last byte which is sum */; i++) {
        sum = (sum + res[i]) % 256;
    }

    if (sum != (unsigned int) res[length - 1]) {
        // sum is wrong
        _errLed.SlowBlinkLed(r1, g1, b1, r2, g2, b2, 60000);
        if (JVS_DEBUG) {
            Serial.println("sum is wrong");
        }
        if (!JVS_PERMISSIVE) {
            free(res);
            return NULL;
        }
    }


    if (!this->handleStatusByte((unsigned char) res[0], r1, g1, b1, r2, g2, b2, 60000)) {
        if (!JVS_PERMISSIVE) {
            free(res);
            return NULL;
        }
    }

    if (!this->handleReportByte((unsigned char) res[1], r1, g1, b1, r2, g2, b2, 60000)) {
        if (!JVS_PERMISSIVE) {
            free(res);
            return NULL;
        }
    }

    resLength = length;
	return res;
}

bool JVSDecoder::timeout(int &counter, unsigned long deadline) {

    if (++counter >= 1000) {
        // reduce calls to millis() by only trying once every N

        counter = 0;

        if (millis() >= deadline) {
            return true;
        }
    }
    return false;
}

// Low level helper function.
// Writes multiple bytes to a JVS destination
// then computes and writes a checksum for error management.
void JVSDecoder::write_packet(int destination, unsigned char data[], int size) {

	this->_JVSSerial.write(SYNC); // SYNC for packet framing
	this->_JVSSerial.write(destination); // JVS device id
	this->_JVSSerial.write(size + 1); // +1 to account for checksum byte

	char sum = destination + size + 1;

	for (int i = 0; i < size; i++) {
		if (data[i] == SYNC || data[i] == MARKBYTE) {
            // these two values are considered special
            // - SYNC always starts a new packet, as such to use it it needs to be escaped
            // - MARK is used for this escape, so to use it itself it also needs to be escaped
            //
            // To escape them, the MARK byte (0xD0) is written, which indicates to the recipient that
            // the next byte should be incremented by 1.
            // As such, to write a byte with the value of SYNC (0xE0), we write: 0xD0 0xDF
            // and to write a byte with the value of MARK (0xD0), we write: 0xD0 0xCF
            //
            // The MARK byte is not considered for the rest of the procotol (length and checksum).
			this->_JVSSerial.write(MARKBYTE);
			this->_JVSSerial.write(data[i] - 1);
		} else {
			this->_JVSSerial.write(data[i]);
		}
		sum = (sum + data[i]) % 256;
	}

	this->_JVSSerial.write(sum);
	this->_JVSSerial.flush();
}
