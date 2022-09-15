#include "constants.h"
#include "decoder.h"
#include "ds2.h"
#include "rgbled.h"
#include "opends2io.h"
#if INPUT_MODE == INPUT_JVS
#include "JVSDecoder.h"
#else
#include "DB15Decoder.h"
#endif

Decoder *decoder = NULL;
unsigned long last = 0;
RGBLed errLed = RGBLed(LED_R_PIN, LED_G_PIN, LED_B_PIN);
DS2 ds2 = DS2(errLed);

void setup()
{

#if INPUT_MODE == INPUT_JVS
    decoder = new JVSDecoder(ds2, errLed);
#else
    decoder = new DB15Decoder(ds2, errLed);
#endif

    if (!JVS_DEBUG) {
        // handle game init first since it is more time sensitive
        while (!ds2.init()) {
        }
    }

    decoder->init();

    last = millis();
}

void loop()
{

    decoder->reset();

    while (1) {

        if (decoder->needReset()) {
            break;
        }

        unsigned long elapsedTime = millis() - last;
        int delayTime = MAIN_LOOP_DELAY - elapsedTime;

        if (JVS_DEBUG) {
            Serial.print("elapsed time since last main loop: ");
            Serial.println(elapsedTime);
        }

        if (delayTime > 0) {
            delay(delayTime);
        }

        last = millis();

        decoder->process(); 

        errLed.Tick(); // yield back to led controller for blink etc.
    }
}
