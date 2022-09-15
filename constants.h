#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

// CUSTOMIZE
#define DS2_UNIQUE_ID       "&12345678" // Your DS2 PCB serial number, to use during the game init handshake
#define DS2_ENABLE_P2       1           // Enable player2 inputs
#define INPUT_MODE          INPUT_JVS   // JVS or DB15
#define JVS_PERMISSIVE      0           // set this to 1 if the init with your JVS I/O board is not going smoothly, to try and force through
#define JVS_SENSE_LOW       200         // value between 0 (0v) and 1023 (5v) to consider that the sense line has gone low
#define JVS_IGNORE_SENSE    0           // ignore sense line for JVS slave detection during JVS init and only init slave id 1
#define JVS_DEBUG           0           // run in debug mode for JVS
                                        // does not interact with the game, instead it uses the USB Serial
                                        // for debug logs

// INPUT MODES
#define INPUT_JVS           0
#define INPUT_DB15          1

// TIMING
#define JVS_DELAY_MASTER_START 	2000
#define JVS_DELAY_SLAVE_START   1000
#define JVS_DELAY_SETADDR       1000
#if JVS_DEBUG == 0
    #define MAIN_LOOP_DELAY     5       // frequent JVS polling, buffered
#else
    #define MAIN_LOOP_DELAY     3000    // slow to make logs inspection easier
#endif
#define DS2_INPUT_DELAY         16      // game is updated every frame

// JVS PINS
#define DE_PIN				PIN_F6
#define SENSE_PIN			PIN_B4

// DB15 pins
#define P1UP_PIN            PIN_B0
#define P1DOWN_PIN          PIN_B1
#define P1LEFT_PIN          PIN_B2
#define P1RIGHT_PIN         PIN_B3
#define P1B1_PIN            PIN_D0
#define P1B2_PIN            PIN_D1
#define P1B3_PIN            PIN_D2
#define P1START_PIN         PIN_D3
#define P1COIN_PIN          PIN_D6
#define P2UP_PIN            PIN_D7
#define P2DOWN_PIN          PIN_B4
#define P2LEFT_PIN          PIN_B5
#define P2RIGHT_PIN         PIN_B6
#define P2B1_PIN            PIN_F7
#define P2B2_PIN            PIN_F6
#define P2B3_PIN            PIN_F5
#define P2START_PIN         PIN_F4
#define P2COIN_PIN          PIN_F1

// rgb led pins
#define LED_R_PIN           PIN_C7
#define LED_G_PIN           PIN_C6
#define LED_B_PIN           PIN_B7

// rgb led colors
#define LED_OFF             0,0,0
#define LED_RED             255,0,0
#define LED_GREEN           0,255,0
#define LED_BLUE            0,0,255
#define LED_YELLOW          255,255,0
#define LED_PINK            255,0,255
#define LED_WHITE           255,255,255
#define LED_DARK_GREEN      0,64,0
#define LED_DARK_RED        64,0,0
#define LED_DARK_BLUE       0,0,64

// JVS

#define SYNC 				0xE0
#define MARKBYTE 			0xD0

#define BROADCAST 			0xFF 	// broadcast address

// broadcast commands
#define CMD_RESET 			0xF0	// reset bus
#define CMD_RESET_ARG		0xD9	// fixed argument to reset command
#define CMD_SETADDR	    	0xF1	// assign address to slave

// single slave commands
#define CMD_IOIDENT         0x10
#define CMD_CMDREV          0x11
#define CMD_JVSREV          0x12
#define CMD_COMMVER         0x13
#define CMD_FEATCHECK       0x14

#define CMD_SWINP        	0x20
#define CMD_COININP 		0x21

// Deathsmiles 2

#define DS2_SERVICE_INPUTS  'P'
#define DS2_P1_INPUTS       'Q'
#define DS2_P2_INPUTS       'R'
#define DS2_COIN_INPUTS     'S'
#define DS2_DELAY_HEARTBEAT 3000    // game timeouts after 6000
#define DS2_DELAY_COINS     100

#define DS2_INPUT_UP        (1<<5)
#define DS2_INPUT_DOWN      (1<<4)
#define DS2_INPUT_LEFT      (1<<3)
#define DS2_INPUT_RIGHT     (1<<2)

#endif /* __CONSTANTS_H__ */
