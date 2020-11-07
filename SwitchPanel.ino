/*
 * Arduino code for modified Logitech Pro Flight Switch Panel using SparkFun Pro Micro
 * 
 * Written by Gavin Lucas
 * https://github.com/GavinLucas/SwitchPanel
 * 
 * Copyright (c) 2019-2020, Gavin Lucas
*/

#include <Joystick.h> // https://github.com/MHeironimus/ArduinoJoystickLibrary

// input used for mags dial
const int magsPin = A3;   // input A3
// values for mags dial
const int magsOff = 0;
const int magsRight = 1;
const int magsLeft = 2;
const int magsBoth = 3;
const int magsStart = 4;
const int numMagsDials = 5;

// input used for fuel dial
const int fuelPin = A2;  // input A2
// values for fuel dial
const int fuelOff = 0;
const int left = 1;
const int both = 2;
const int right = 3;
const int numFuelDials = 4;

// switches
const int numSwitches = 14;
const int switchPin[numSwitches] = {1, 0, 2, 3, 4, 7, 8, 9, 10, 16, 14, 15, 18, 19};
const int bat = 0;      // input 1
const int alt = 1;      // input 0
const int avi = 2;      // input 2
const int pump = 3;     // input 3
const int deIce = 4;    // input 4
const int pitot = 5;    // input 7
const int cowl = 6;     // input 8
const int panel = 7;    // input 9
const int beacon = 8;   // input 10
const int nav = 9;      // input 16
const int strobe = 10;  // input 14
const int taxi = 11;    // input 15
const int landing = 12; // input 18 
const int gear = 13;    // input 19

// gear LED pins
const int redPin = 5;   // input 5
const int greenPin = 6; // input 6

// gear LED values
const int off = 0;
const int red = 1;
const int green = 2;
const int yellow = 3;

// gear state values
const bool up = false;
const bool down = true;

// initialise some other values
const int numVirtButtons = (numSwitches + numMagsDials + numFuelDials);
const unsigned long gearTransitionTime = 5000; // the time it takes to extend and retract the gear
const unsigned long virtButtonPress = 100;    // the duration of the button press
const unsigned long debounceTime = 20;       // duration to ignore further input

// declare some variables

int prevMagsPosition;
int prevFuelPosition;
bool switchLastState[numSwitches]; // the previous state will be stored here
unsigned long initButtonPress[numVirtButtons] = { 0 };  // the time the output pin was last toggled

// define the stick's configuration
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK,
  numVirtButtons, 0,     // Button Count, Hat Switch Count
  false, false, false,   // No X, Y, or Z Axis
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering

// initial setup
void setup() {
  // setup mags input
  pinMode(magsPin, INPUT);
  prevMagsPosition = readAnalogPosition(magsPin); // the previous state will be stored here
  
  // setup fuel input
  pinMode(fuelPin, INPUT);
  prevFuelPosition = readAnalogPosition(fuelPin); // the previous state will be stored here
  
  // setup switches
  for (int i=0; i < numSwitches; i++) {
    pinMode(switchPin[i], INPUT_PULLUP);
    switchLastState[i] = digitalRead(switchPin[i]);
  }

  // setup LEDs
  pinMode(redPin, OUTPUT);
  digitalWrite(redPin, HIGH); // HIGH is off, LOW is on
  
  pinMode(greenPin, OUTPUT);
  digitalWrite(greenPin, HIGH); // HIGH is off, LOW is on

  if (digitalRead(switchPin[bat]) == LOW && digitalRead(switchPin[gear]) == down) {
    setLEDcolour(green);
  }
  
  // initialize Joystick Library
  Joystick.begin();
}

// main loop
void loop() {
  // time loop started to avoid constantly calling millis() in the loop
  unsigned long loopMillis = millis(); 

  // read the mags state
  int magsPosition = readAnalogPosition(magsPin);
  if (magsPosition != prevMagsPosition && magsPosition < numMagsDials) {
    int mbtn = numSwitches + magsPosition;
    int oldmbtn = numSwitches + prevMagsPosition;
    Joystick.setButton(oldmbtn, LOW);
    Joystick.setButton(mbtn, HIGH);
    initButtonPress[mbtn] = loopMillis;
    prevMagsPosition = magsPosition;
  }
  // release the mags button if it's time (apart from the start button)
  int magbtn = numSwitches + prevMagsPosition;
  if (prevMagsPosition != magsStart && ((initButtonPress[magbtn] + virtButtonPress) < loopMillis)) {
    Joystick.setButton(magbtn, LOW);
  }

  // read the fuel state
  int fuelPosition = readAnalogPosition(fuelPin);
  if (fuelPosition != prevFuelPosition && fuelPosition < numFuelDials) {
    int fbtn = numSwitches + numMagsDials + fuelPosition;
    int oldfbtn = numSwitches + numMagsDials + prevFuelPosition;
    Joystick.setButton(oldfbtn, LOW);
    Joystick.setButton(fbtn, HIGH);
    initButtonPress[fbtn] = loopMillis;
    prevFuelPosition = fuelPosition;
  }
  // release the fuel button if it's time
  int fuelbtn = numSwitches + numMagsDials + prevFuelPosition;
  if ((initButtonPress[fuelbtn] + virtButtonPress) < loopMillis) {
    Joystick.setButton(fuelbtn, LOW);
  }
  
  // read switch states
  for (int sw=0; sw < numSwitches; sw++) {
    // read the switch state
    bool switchState = digitalRead(switchPin[sw]);
    if (switchState != switchLastState[sw] && (initButtonPress[sw] + debounceTime) < loopMillis) {
      // output the state to the PC
      Joystick.setButton(sw, HIGH);
      switchLastState[sw] = switchState;
      initButtonPress[sw] = loopMillis;
    }
    // release the button if it's time
    else if ((initButtonPress[sw] + virtButtonPress) < loopMillis) {
      Joystick.setButton(sw, LOW);
    }
  }

  // set LED colours if the battery switch is on, else lights are off
  if (switchLastState[bat] == HIGH || (switchLastState[gear] == up && (loopMillis - initButtonPress[gear]) > gearTransitionTime)) {
    setLEDcolour(off);
  }
  else if (initButtonPress[gear] != 0 && (loopMillis - initButtonPress[gear]) < gearTransitionTime) {
    setLEDcolour(red);
  }
  else if (switchLastState[gear] == down && (loopMillis - initButtonPress[gear]) > gearTransitionTime) {
    setLEDcolour(green);
  }
}

// function to read a resistor ladder
int readAnalogPosition(int inputPin) {
  int analogPosition = analogRead(inputPin);
  if (analogPosition < 256) return 0;
  if (analogPosition < 598) return 1;
  if (analogPosition < 726) return 2;
  if (analogPosition < 794) return 3;
  if (analogPosition < 921) return 4;
  if (analogPosition > 921) return 5;
}

// function to set the LED colour
void setLEDcolour(int LEDcolour) {
  switch (LEDcolour) {
    case off:
      digitalWrite(greenPin, HIGH);
      digitalWrite(redPin, HIGH);
      break;
    case red:
      digitalWrite(greenPin, HIGH);
      digitalWrite(redPin, LOW);
      break;
    case green:
      digitalWrite(greenPin, LOW);
      digitalWrite(redPin, HIGH);
      break;
    case yellow:
      digitalWrite(greenPin, LOW);
      digitalWrite(redPin, LOW);
      break;
  }
}
