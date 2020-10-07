#include <Joystick.h> // https://github.com/MHeironimus/ArduinoJoystickLibrary

// input used for mags dial
const int magsPin = A3;
// values for mags dial
const int magsOff = 0;
const int magsRight = 1;
const int magsLeft = 2;
const int magsBoth = 3;
const int magsStart = 4;
const int numMagsDials = 5;

// input used for fuel dial
const int fuelPin = A2;
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
const int redPin = 5;
const int greenPin = 6;

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

// and declare some variables
unsigned long loopMillis; // the time the loop starts
unsigned long initButtonPress[numVirtButtons] = { 0 };  // the time the output pin was last toggled
unsigned long gearChangeTime = 0;
bool gearState;
bool batState;
int magsPosition; // the position of the mags dial
int prevMagsPosition;
int fuelPosition; // the position of the fuel dial
int prevFuelPosition;
bool switchLastState[numSwitches]; // the previous state will be stored here

// define the stick's configuration
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK,
  numVirtButtons, 0,     // Button Count, Hat Switch Count
  false, false, false,   // No X, Y, or Z Axis
  false, false, false,   // No Rx, Ry, or Rz
  false, false,          // No rudder or throttle
  false, false, false);  // No accelerator, brake, or steering

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

void loop() {
  // time loop started to avoid constantly calling millis() in the loop
  loopMillis = millis();
  
  // read the mags state
  magsPosition = readAnalogPosition(magsPin);
  if (magsPosition != prevMagsPosition && magsPosition < magsStart) {
    Joystick.setButton((numSwitches + magsStart), LOW);
    Joystick.setButton((numSwitches + magsPosition), HIGH);
    delay(virtButtonPress);
    Joystick.setButton((numSwitches + magsPosition), LOW);
    prevMagsPosition = magsPosition;
  }
  else if (magsPosition != prevMagsPosition && magsPosition == magsStart) {
    Joystick.setButton((numSwitches + magsStart), HIGH);
    prevMagsPosition = magsPosition;
  }

  // read the fuel state
  fuelPosition = readAnalogPosition(fuelPin);
  if (fuelPosition != prevFuelPosition) {
    Joystick.setButton((numSwitches + numMagsDials + fuelPosition), HIGH);
    delay(virtButtonPress);
    Joystick.setButton((numSwitches + numMagsDials + fuelPosition), LOW);
    prevFuelPosition = fuelPosition;
  }
  
  // read switch states
  for (int i=1; i < (numSwitches - 1); i++) {
    // read the switch state
    bool switchState = digitalRead(switchPin[i]);
    if (switchState != switchLastState[i]) {
      // output the state to the PC
      Joystick.setButton(i, HIGH);
      delay(virtButtonPress);
      Joystick.setButton(i, LOW);
      switchLastState[i] = switchState;
    }
  }

  //read bat switch state
  batState = digitalRead(switchPin[bat]);
  if (batState != switchLastState[bat]) {
    // output the state to the PC
    Joystick.setButton(bat, HIGH);
    delay(virtButtonPress);
    Joystick.setButton(bat, LOW);
    switchLastState[bat] = batState;
  }

  // read gear switch state
  gearState = digitalRead(switchPin[gear]);
  if (gearState != switchLastState[gear]) {
      // output the state to the PC
      Joystick.setButton(gear, HIGH);
      delay(virtButtonPress);
      Joystick.setButton(gear, LOW);
      switchLastState[gear] = gearState;
      gearChangeTime = loopMillis;
  }
  
  // set LED colours
  if (batState == true || (gearState == up && (loopMillis - gearChangeTime) > gearTransitionTime)) {
    setLEDcolour(off);
  }
  else if (gearChangeTime != 0 && (loopMillis - gearChangeTime) < gearTransitionTime) {
    setLEDcolour(red);
  }
  else if (gearState == down && (loopMillis - gearChangeTime) > gearTransitionTime) {
    setLEDcolour(green);
  }
}

int readAnalogPosition(int inputPin) {
  int analogPosition = analogRead(inputPin);
  if (analogPosition < 256) return 0;
  if (analogPosition < 598) return 1;
  if (analogPosition < 726) return 2;
  if (analogPosition < 794) return 3;
  if (analogPosition < 921) return 4;
  if (analogPosition > 921) return 5;
}

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
