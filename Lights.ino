/*  Smart Traffic Light (LDR-driven) — Arduino Uno R3
    - Day mode: normal traffic cycle (GREEN → YELLOW → RED)
    - Night mode: flashing YELLOW (caution). Optional PIR gives a brief GREEN.
    - Non-blocking (millis), exponential smoothing, hysteresis to prevent flicker.

    Pins (change as needed):
      A0  : LDR voltage divider
      D8  : RED LED
      D9  : YELLOW LED
      D10 : GREEN LED
      D2  : PIR (optional)

    Based on project description: Smart Traffic Light System Controlled by Light Sensor. 
*/

#include <Arduino.h>

// ==========================
//           PINS
// ==========================
// These are the Arduino pins your parts connect to.
// If your LEDs are on different pins, just change the numbers here.
const uint8_t PIN_LDR      = A0;   // LDR (photoresistor) voltage divider junction
const uint8_t PIN_LED_RED  = 8;    // Red LED (through a 220Ω resistor to GND)
const uint8_t PIN_LED_YEL  = 9;    // Yellow LED (through a 220Ω resistor to GND)
const uint8_t PIN_LED_GRN  = 10;   // Green LED (through a 220Ω resistor to GND)
const uint8_t PIN_PIR      = 2;    // Optional PIR motion sensor (OUT pin)

// Turn PIR support on/off here. Leave false if you don't have a PIR connected.
const bool USE_PIR = false;

// LDR Filter & Thresholds
// LDR_ALPHA: 0..1 (higher = faster response, lower = smoother).
const float LDR_ALPHA = 0.12f;

//Use 2 thresholds so the mode doesn't keep flipping back and forth.
int DAY_ENTER   = 650;  // brighter than this → day
int NIGHT_ENTER = 450;  // darker than this  → night

// ==========================
//         Timing (ms)
// ==========================
// How long each light stays on (in milliseconds).
const unsigned long DAY_GREEN_MS   = 6000; // 6 seconds green
const unsigned long DAY_YELLOW_MS  = 2000; // 2 seconds yellow
const unsigned long DAY_RED_MS     = 4000; // 4 seconds red

// Night mode: flash yellow every NIGHT_FLASH_MS milliseconds.
const unsigned long NIGHT_FLASH_MS = 600;  // blink speed at night

//(Optional) When using PIR at night, do a short cycle so a car/person can go.
const unsigned long N_PASS_GREEN_MS  = 4000; // 4 seconds green
const unsigned long N_PASS_YELLOW_MS = 1500; // 1.5 seconds yellow
const unsigned long N_PASS_RED_MS    = 2500; // 2.5 seconds red

// ==========================
//      MODES & STATES
// ==========================
// MODE = are we in Day or Night?
// LightState = which light pattern is currently active?
enum Mode { MODE_DAY, MODE_NIGHT };
enum LightState {
  L_GREEN,
  L_YELLOW,
  L_RED,
  L_FLASH_YELLOW,   // Night-only: blink yellow on/off
  L_NPASS_GREEN,    // Night-only: PIR short cycle (green)
  L_NPASS_YELLOW,   // Night-only: PIR short cycle (yellow)
  L_NPASS_RED       // Night-only: PIR short cycle (red), then back to flash
};

Mode mode = MODE_DAY;            // current mode (starts decided in setup)
LightState lightState = L_RED;   // current light state

// "Timers" based on ms() so we never block with delay()
unsigned long stateStart = 0;      // when did we enter the current state?
unsigned long lastFlashToggle = 0; // last time we toggled the night flash
bool flashOn = false;              // is the yellow currently lit during flash?

// Smoothed LDR value (float) so we can use the exponential filter
float ldrFiltered = 0;

// ==========================
//   LIGHT CONTROL FUNCTIONS
// ==========================

// Turn specific LEDs on/off in one go.
// Usage: setLights(redOn, yellowOn, greenOn);
void setLights(bool r, bool y, bool g) {
  digitalWrite(PIN_LED_RED,   r ? HIGH : LOW);
  digitalWrite(PIN_LED_YEL,   y ? HIGH : LOW);
  digitalWrite(PIN_LED_GRN,   g ? HIGH : LOW);
}

// Switch to a given light state and remember WHEN it happened.
void enterState(LightState s) {
  lightState = s;
  stateStart = millis();

  switch (s) {
    case L_GREEN:         setLights(false, false, true);  break;  // Green ON
    case L_YELLOW:        setLights(false, true,  false); break;  // Yellow ON
    case L_RED:           setLights(true,  false, false); break;  // Red ON
    case L_FLASH_YELLOW:  /* flashing handled in loop() */ break; // We'll blink yellow
    case L_NPASS_GREEN:   setLights(false, false, true);  break;  // Night pass: Green
    case L_NPASS_YELLOW:  setLights(false, true,  false); break;  // Night pass: Yellow
    case L_NPASS_RED:     setLights(true,  false, false); break;  // Night pass: Red
  }
}

// Change between DAY and NIGHT modes.
// - In DAY: start regular cycle (Green)
// - In NIGHT: prepare for blinking yellow
void enterMode(Mode m) {
  mode = m;
  if (mode == MODE_DAY) {
    enterState(L_GREEN);  // Start day cycle on green
  } else { // MODE_NIGHT
    flashOn = false;                        // start with yellow OFF
    lastFlashToggle = millis();             // reset flash timer
    setLights(false, false, false);         // make sure all LEDs are off
    enterState(L_FLASH_YELLOW);             // go to flashing yellow state
  }
}

// ==========================
//           SETUP
// ==========================
// Runs once after power-up/reset.
void setup() {
  // Set LED pins to OUTPUT so we can turn them on/off.
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_YEL, OUTPUT);
  pinMode(PIN_LED_GRN, OUTPUT);

  // LDR is an analog input (voltage divider). No special mode needed.
  pinMode(PIN_LDR, INPUT);

  // PIR (optional). Most common modules (HC-SR501) drive HIGH/LOW themselves,
  // so INPUT is fine. If your sensor has an open-collector output, use INPUT_PULLUP.
  if (USE_PIR) pinMode(PIN_PIR, INPUT);

  // Make sure all LEDs start OFF.
  setLights(false, false, false);

  // Start serial so we can see values in Tools → Serial Plotter (9600 is fine).
  Serial.begin(9600);
  delay(50); // small pause so Serial is ready on some boards

  // // Average 20 LDR readings to smooth out startup noise
  long sum = 0;
  for (int i = 0; i < 20; i++) {
    sum += analogRead(PIN_LDR);
    delay(5);
  }
  ldrFiltered = sum / 20.0;

  // Decide initial mode right now based on the current light level.
  // If it's bright → day, if it's dark → night.
  enterMode(ldrFiltered >= DAY_ENTER ? MODE_DAY : MODE_NIGHT);
}