# Deathsmiles 2  error codes

These are guesses on the meaning/cause of the game's lockup / error screens, based on the behavior observed during testing.

- 4001: the init sequence (handshake with the control board) did not complete correctly.
- 4003: the control board failed to heartbeat in time
- 8000: seems to happen when the frequency of control commands is too high.

# opends2io error codes

Errors are communicated using the one RGB led in the circuit.
It will be set to a solid color, blink on/off on a single color, or blink alternating between 2 colors to signify an error code.
Furthermore, the blinking speed can give more insight as to what went wrong.

There are 4 speeds:
- Solid: no blinking
- Slow (blink every 3 seconds)
- Normal (blink every second)
- Fast (blink 3 times per second)

After a while (between 10 seconds and 1 minute, depending on the error), the LED goes back to normal.

## Game init / handshake

### SLOW BLINKING YELLOW

The game answered something unexpected during the first part of the init handshake with the controls board.

### NORMAL BLINKING YELLOW

The game answered something unexpected during the second part of the init handshake with the controls board.

## Controls & coins

### SLOW BLINKING RED

Bad SUM or STATUS when we request the JVS slave for controls & coins state.
This could be due to: a transmission issue, defective JVS I/O board, a bug in opends2io (wrong command sent or wrong checksum), or an overflow (too much data in response).

### NORMAL BLINKING RED

Bad REPORT when we request the JVS slave for controls & coins state.
This could be due to: a bug in opends2io (invalid data supplied in request or invalid number of parameters), or the device was busy.

### NORMAL BLINKING RED / DARKER RED

Bad REPORT when we request the JVS slave for controls & coins state.
This could be due to: a bug in opends2io (invalid data supplied in request or invalid number of parameters), or the device was busy.

### FAST BLINKING RED / DARKER RED

Bad coin state when we request the JVS slave for controls & coins state.
This could be due to: a coin jam, or the device was busy.

## JVS init

### SOLID BLUE

Timeout during JVS init.

### SLOW BLINKING BLUE / GREEN

Bad SUM or STATUS when we request the JVS slave to set an address (SETADDR).
This could be due to: a transmission issue, defective JVS I/O board, a bug in opends2io (wrong command sent or wrong checksum), or an overflow (too much data in response).

### NORMAL BLINKING BLUE / GREEN

Bad REPORT when we request the JVS slave to set an address (SETADDR).
This could be due to: a bug in opends2io (invalid data supplied in request or invalid number of parameters), or the device was busy.

### SLOW BLINKING BLUE / PINK

Bad SUM or STATUS when we request the JVS slave to identify itself (IOIDENT).
This could be due to: a transmission issue, defective JVS I/O board, a bug in opends2io (wrong command sent or wrong checksum), or an overflow (too much data in response).

### NORMAL BLINKING BLUE / PINK

Bad REPORT when we request the JVS slave to identify itself (IOIDENT).
This could be due to: a bug in opends2io (invalid data supplied in request or invalid number of parameters), or the device was busy.

### SLOW BLINKING BLUE / WHITE

Bad SUM or STATUS when we request the JVS slave its supported JVS command format revision (CMDREV).
This could be due to: a transmission issue, defective JVS I/O board, a bug in opends2io (wrong command sent or wrong checksum), or an overflow (too much data in response).

### NORMAL BLINKING BLUE / WHITE

Bad REPORT when we request the JVS slave its supported JVS command format revision (CMDREV).
This could be due to: a bug in opends2io (invalid data supplied in request or invalid number of parameters), or the device was busy.

### SLOW BLINKING BLUE / DARKER BLUE

Bad SUM or STATUS when we request the JVS slave its supported JVS revision (JVSREV).
ehis could be due to: a transmission issue, defective JVS I/O board, a bug in opends2io (wrong command sent or wrong checksum), or an overflow (too much data in response).

### NORMAL BLINKING BLUE / DARKER BLUE

Bad REPORT when we request the JVS slave its supported JVS revision (JVSREV).
This could be due to: a bug in opends2io (invalid data supplied in request or invalid number of parameters), or the device was busy.

### SLOW BLINKING BLUE

Bad SUM or STATUS when we request the JVS slave its supported JVS communication system revision (COMMVER).
ehis could be due to: a transmission issue, defective JVS I/O board, a bug in opends2io (wrong command sent or wrong checksum), or an overflow (too much data in response).

### NORMAL BLINKING BLUE

Bad REPORT when we request the JVS slave its supported JVS communication system revision (COMMVER).
This could be due to: a bug in opends2io (invalid data supplied in request or invalid number of parameters), or the device was busy.

### SLOW BLINKING GREEN

Bad SUM or STATUS when we request the JVS slave the features it supports (FEATCHECK).
ehis could be due to: a transmission issue, defective JVS I/O board, a bug in opends2io (wrong command sent or wrong checksum), or an overflow (too much data in response).

### NORMAL BLINKING GREEN

Bad REPORT when we request the JVS slave the features it supports (FEATCHECK).
This could be due to: a bug in opends2io (invalid data supplied in request or invalid number of parameters), or the device was busy.

### SLOW BLINKING GREEN / DARKER GREEN

The JVS slave does not support digital inputs (joystick / buttons).

### NORMAL BLINKING GREEN / DARKER GREEN

The JVS slave does not support enough buttons (1 lever + 3 buttons).

### FAST BLINKING GREEN / DARKER GREEN

The JVS slave does not support any coin slot.
