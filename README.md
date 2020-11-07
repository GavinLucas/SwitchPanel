# Arduino code for modified Logitech Pro Flight Switch Panel using SparkFun Pro Micro
#### Copyright (c) 2019-2020, Gavin Lucas
#### https://github.com/GavinLucas/SwitchPanel

This sketch for a modified Logitech Pro Flight Panel with a SparkFun Pro Micro Arduino board replacing the original board.

It also adds an extra 4 position fuel selector switch.

LEDs are wired in parallel with six individual current limiting resistors.  If the master switch is on then these are cycled using a timer triggered by the landing gear switch.  They are for decorative purposes only and do not show the actual status of the landing gear. 

Mags and Fuel swithes use 1k resistor ladders on analog ports with an external 1k pull-up resistor in order to have sufficient inputs and outputs.

## Dependencies
### Arduino Joystick Library
This Arduino sketch file depends on the [Arduino Joystick Library](https://github.com/MHeironimus/ArduinoJoystickLibrary)
