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