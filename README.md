# SmartRelay

It's based in Attiny85 ans it allows to command relay module through, both, a push of a button and commands through a standard serial line. It has three modes:
* always-on: the relay is always on all the time and ignores the relay commands
* always-off:  the relay is always off all the time and ignores the relay commands
* auto: the relay is controlled through serial commands.

When the button is pushed, the mode is reported through a serie of led blinks (like the morse codes) by a simple button push. The blink codes are:
* always-on:  3 cycles of "800ms ON, 200ms OFF"
* always-off: 3 cycles of "200ms ON, 800ms OFF"
* auto: 3 cycles of "500ms ON, 500ms OFF".

If the button is pushed while it's in present mode (led blinks) then the mode cycles.

The hardware requires a usb to ttl cheap dongle (like PL2303HX USB To RS232 TTL) which allows the communication with the microcontroller through a serial interface. The seriam commands are:
* gm: get mode. It replyes 0 for auto, 1 for always-on and 2 for always-off
* sm <relay_mode>: set mode, where the mode is a number, as described above
* grs: get relay state. Replays 1 for relay state ON and 0 for relay state OFF
* srs <relay_state>: sets the relay state, only if the mode is auto. It's followd by the relay state number (0 or 1).

The microcontroller's pins are described in the main cpp:
* PIN0[IN]: Serial Rx (for using analog external interrupt)
* PIN1[OUT]: led pin (Digispark dev board)
* PIN2[IN_PULLUP]: mode change push button pin (sole ext. int. pin)
* PIN3[OUT]: Serial Tx
* PIN4[OUT]: output relay drive (drives the actual load)
* PIN5[IN]: unused - reserver for reset/debugWire.
