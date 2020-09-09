# SwitchPanel
Code for Arduino based switch panel.

This is code for a modified Logitech Pro Flight Panel with a SparkFun Pro Micro Arduino board replacing the original board.

It also adds an extra fuel selector switch and controls the LEDs on a timer.

LEDs are wired in parallel with six individual current limiting resistors.

Mags and Fuel swithes use 1k resistor ladders on analog ports with an external 1k pull-up resistor in order to have sufficient inputs and outputs.
