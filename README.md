# 🚦 Smart Traffic Light (Arduino + LDR + LEDs)

---

## 📌 Project Description

A simple **Arduino Uno** project that simulates a traffic light.

It uses an **LDR (light-dependent resistor)** to switch between:

- **Day mode** → normal cycle (Green → Yellow → Red)  
- **Night mode** → flashing Yellow  
- *(Optional)* A **PIR sensor** can trigger a short cycle at night when motion is detected  

---

## 🛠️ Hardware Used

- Elegoo Uno R3 (Arduino compatible)  
- Breadboard + jumper wires  
- 3 LEDs (Red, Yellow, Green)  
- 3 × 220Ω resistors (for LEDs)  
- LDR (light sensor)  
- 10kΩ resistor (for LDR voltage divider)  
- *(Optional)* PIR motion sensor  

--- 

**LEDs**  
- Red → Pin 8 (with resistor to GND)  
- Yellow → Pin 9 (with resistor to GND)  
- Green → Pin 10 (with resistor to GND)  

**LDR**  
- One leg → 5V  
- Other leg → A0 **and** 10kΩ resistor to GND  

**PIR sensor (optional)**  
- VCC → 5V  
- GND → GND  
- OUT → Pin 2  

---

## 💻 Software Setup

1. Install [Arduino IDE](https://www.arduino.cc/en/software).  
2. Clone this repository:  
   ```bash
   git clone https://github.com/SamuelOgundele/Arduino.git
1. Open **`Arduino/light.ino`** in the Arduino IDE.  
2. Select **Board: Arduino Uno** (or Elegoo Uno R3).  
3. Click **Upload** to flash the code to your board.  

---

## 📊 Calibrating the LDR

1. Open **Tools → Serial Plotter** in the Arduino IDE.  
2. Make sure the baud rate (bottom right) is set to **9600**.  
3. You will see three lines:  
   - The live **LDR reading**  
   - The **DAY_ENTER threshold**  
   - The **NIGHT_ENTER threshold**  
4. Adjust the threshold numbers in the code so the Arduino switches between **day** and **night** at the correct light level for your environment.  

---

## 🚦 Modes of Operation

- **Day mode**  
  - Green → Yellow → Red cycle repeats  

- **Night mode**  
  - Yellow blinks on/off  

- **Night + PIR (if connected)**  
  - Motion detected → short Green → Yellow → Red cycle → back to blinking Yellow  

---
