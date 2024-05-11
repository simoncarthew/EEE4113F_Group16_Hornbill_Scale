/*
* Project: Southern Ground Hornbill Monitoring System
* Module: Weighing Scale Calibration Script (ESP32)
* 
* Description: This ESP32 code forms the calibration process for the weighing scale 
*              used in the Southern Ground Hornbill monitoring system for the EEE4113F 
*              Design Course at UCT. Its primary functions include:
*
*   1. Load Cell Calibration: Calibrates the load cell to ensure accurate weight readings.
*   2. Tare Function: Sets the scale to zero to account for any existing weight on the scale.
*   3. Weight Reading: Takes a weight reading from the scale for calibration purposes.
*   4. Calibration Factor Calculation: Calculates the calibration factor based on the known weight.
*
* Author: Travimadox Webb
* Date: 2024-04-18
* Version: 1.0
*/

/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-load-cell-hx711/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

// Calibrating the load cell
#include <Arduino.h>
#include "soc/rtc.h"
#include "HX711.h"

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 27;
const int LOADCELL_SCK_PIN = 4;

HX711 scale;

// Known weight for calibration
const float known_weight = 1000; // grams. Can be changed depending on the available weight

void setup() {
  Serial.begin(115200);
  rtc_cpu_freq_config_t config;
  rtc_clk_cpu_freq_get_config(&config);
  rtc_clk_cpu_freq_to_config(RTC_CPU_FREQ_80M, &config);
  rtc_clk_cpu_freq_set_config_fast(&config);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
}

void loop() {

  if (scale.is_ready()) {
    scale.set_scale();    
    Serial.println("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare();
    Serial.println("Tare done...");
    Serial.print("Place a known weight on the scale...");
    delay(5000);
    long reading = scale.get_units(10);
    Serial.print("Result: ");
    Serial.println(reading);
    Serial.println("Remove the weight from the scale.");        
    // Calculate and print the calibration factor
    float calibration_factor = reading / known_weight;
    Serial.print("Calibration factor: ");
    Serial.println(calibration_factor);
  } 
  else {
    Serial.println("HX711 not found.");
  }
  delay(1000);
}