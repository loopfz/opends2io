# opends2io

`opends2io` is an open source reimplementation of the I/O control board for the arcade game Deathsmiles II (CAVE).
It is based on the Teensy 2.0 board, but could maybe support other microcontrollers in the future.


## Overview

`opends2io` consists of:
- a program to upload to a Teensy 2.0 board. This program understands the JVS standard as input and implements the control protocol specific to Deathsmiles II.
- a PCB design replicating the layout of the original cv2000xp board, on which you can solder the Teensy 2.0 board and the additional components.

![opends2io PCB](/pcb/pcb_opends2io.png)


## Deathsmiles II

Released in 2009, Deathsmiles II was the only CAVE game based on PC hardware (named "CAVE pc"), which is now the norm for modern arcade games.

It used regular off-the-shelf components:
- CPU: AMD Athlon X2 5050e or AMD Athlon X2 7750
- Motherboard: Asus M3A78-EM or Gigabyte GA-MA78GPM-UD2H
- RAM: 2GB DDR2 800
- Sound card, graphics card: none (integrated in the motherboard)
- Controls: cv2000xp, proprietary card (the goal of this project)
- Security: USB dongle with unique data for each serial number

The cv2000xp control board expects JVS coming from the arcade cabinet. It then translates it into a custom serial protocol between the control board and the game. This logic is what is being reimplemented by `opends2io`.

The cv2000xp control board and the USB dongle are both unique for each unit / serial number of the game.
The game executable will read data from both and make sure the serial number matches.

The game runs on a stripped down Windows XP Embedded.
The original game image has been dumped and is part of the MAME CHDs.
Do note that there are supposedly two distinct images: one for Gigabyte motherboards, one for Asus motherboards. They are not compatible. The MAME CHD is the Gigabyte image.


## What you can do with this project

If you own the original Deathsmiles II hardware and your controls board is fried, you can use the `opends2io` PCB design and code for a drop-in replacement.
The PCB design is modeled after the original to fit in the same location over the motherboard.
Since this is the only proprietary component in the original hardware (the rest being off the shelf PC parts), it means everything can be replaced now.

`opends2io` can also be built and configured without JVS input, using a regular DB15 input with neo geo pinout instead.
This can be made into a small adapter box to consolize Deathsmiles 2, using e.g. a neo geo pad or a USB2DB15. PCB design and 3d-printable box to come for this.


## Current state

`opends2io` is currently in ALPHA and not yet considered stable.

What works:
- Credits
- Controls, P1 & P2
- Initialization sequence and regular heartbeat

Basically, the game can be played fully. It's still early and unexpected issues may arise though.

Here are the various aspects I'd like to focus on next, in no particular order:

### TODO #1: Input latency

The game is a bit finicky about the frequency of commands it receives, apparently. In testing, things did not go well when I pushed to send updates as quickly / frequently as possible.
I've tried to fiund a satisfactory middle ground, but it needs further testing.

Getting a few testers who are experts at the game could help detect/fix issues in input handling.

Inspecting the behavior of the original cv2000xp board with regard to the frequency of controls updates would also help a lot.

### TODO #2: Seamless integration with the original game image

A few things need to be done so that the open source board will be recognized by the original game image and assigned the same port without the user needing to fiddle with it, so the game can find it.

I'm unable to test this right now.

[This thread on cave-stg](http://cave-stg.com/forum/index.php?topic=472.msg33100#msg33100) details the incompatibility between the original images (one for the asus motherboard, one for the gigabyte motherboard).
The MAME CHD is for the gigabyte motherboard.

I have a test rig for this purpose, but unfortunately it has the Asus motherboard. Without access to the Asus original image I may not be able to work on + test this integration.

### TODO #3: Previous revisions of DS2

`opends2io` has been tested with the final revision (4.0) of Deathsmiles 2.

I have no access to previous revisions as of now and cannot test how the board behaves with them.

### TODO #4: USB dongle

Each original controls board was uniquely programmed with the PCB's serial number as part of a security measure.
It is passed to the game and compared with the contents of the USB security dongle containing a matching key.

Without access to the contents of a USB dongle I can't test if this works fine.

This check can be disabled by modifying a few bytes in `cvgame.exe`, which is how my testing has been done so far.

### TODO #5: Check PCB features

I need to confirm a few things about the PCB design relative to the original cv2000xp board.

For example:
- is the pinout for the F_PANEL header correct?
- what is the purpose of the pushbutton that is close to the F_PANEL header?
  I assumed it was a power switch and went ahead and included one in my design since it seemed convenient, but it could be something else entirely.


## Help!

If you can/want to help with the above points, reach out to loopfz AT gmail. It would be very appreciated :)


## Building opends2io

### PCB

TODO

### Uploading to Teensy 2.0 board

TODO


## Acknowledgements

This project started out as a fork of [JVSy](https://github.com/k4roshi/JVSy) which interfaces JVS to a computer, acting as a standard gamepad. Most of the code has changed/evolved a lot, but the outline of the program remains. Great project, it was very helpful.

[JVS2x](https://github.com/Fredobedo/JVS2X) is another great fork of `JVSy` that was a good source of inspiration for the handling of the JVS sense line.

`yksoft1` shared insights regarding the Deathsmiles 2 serial protocol in various forum threads and also several patches to `cvgame.exe` to bypass some limitations (usb dongle, heartbeat timeout etc.), making this project possible in the first place.

Other important ressources used:
- [Translated JVS reference](http://daifukkat.su/files/jvs_wip.pdf): essential to understand the finer details of the JVS protocol
- [Photos of the inside of Deathsmiles II boxes](https://ikotsu.blogspot.com/2010/03/deathsmiles-ii-pos-arcade-pc.html): helped me design the PCB (layout, how it is attached, pinout of the F_PANEL header, etc.)
- Many forum threads e.g. [Cave-STG](http://cave-stg.com/forum/index.php?topic=472.420), [Neofighters](https://www.neofighters.info/forum/showthread.php?14783-Deathsmiles-2-Arcade-PC), ...


## License

The code and pcb are both licensed under GNU GPL v3.
See [LICENSE](LICENSE).
