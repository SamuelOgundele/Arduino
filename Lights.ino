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