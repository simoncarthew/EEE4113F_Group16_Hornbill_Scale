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
const int LOADCELL_DOUT_PIN = 18;
const int LOADCELL_SCK_PIN = 5;

HX711 scale;

bool calibrated;
bool weight_recieved;

void setup() {
  Serial.begin(921600);
  rtc_cpu_freq_config_t config;
  rtc_clk_cpu_freq_get_config(&config);
  rtc_clk_cpu_freq_to_config(RTC_CPU_FREQ_80M, &config);
  rtc_clk_cpu_freq_set_config_fast(&config);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  calibrated = false;
}

void loop() {
  while (calibrated == false){ // repeat until user validates that the scale is calibrated
    if (scale.is_ready()) { // check if the scale is ready
      scale.set_scale(); // set the scale value to 1

      // find the zero point of the scale  
      Serial.println("Tare... remove any weights from the scale.");
      delay(2500);
      scale.tare();
      Serial.print("Offset = ");
      Serial.println(scale.get_offset());

      // read in the known weight from the user
      int known_weight = 0;
      weight_recieved = false;
      Serial.println("Please enter the known weight in grams.");
      while (!weight_recieved) {
        // Check if there is data available to read
        if (Serial.available() > 0) {
            // Read the incoming character
            char incomingChar = Serial.read();

            // Check if the incoming character is a digit
            if (isdigit(incomingChar)) {
                // Convert the character to an integer and update receivedNumber
                known_weight = known_weight * 10 + (incomingChar - '0');
            } else if (incomingChar == '\n') {
                // If newline character is received, it indicates the end of the number
                weight_recieved = true; // Set flag to indicate that a number has been received
            }
        }
      }
      Serial.print("Entered weight: ");
      Serial.print(known_weight);
      Serial.println(" g");
      delay(500);

      // calibrate using the known weight entered
      Serial.println("Place the known weight on the scale...");
      delay(2500);
      long reading = scale.get_units(10);
      Serial.print("Result: ");
      Serial.println(reading);
      float cal_factor = float(reading)/float(known_weight);
      Serial.print("Calibration Factor = ");
      Serial.println(cal_factor);
      scale.set_scale(cal_factor);
      delay(500);

     // Check if the user is happy with the calibration
      Serial.println("Are you happy with the calibration, y/n?");
      char receivedChar;
      bool receivedYN = false;

      // Loop until a valid character is received
      while (!receivedYN) {
        // Check if there is data available to read
        if (Serial.available() > 0) {
          // Read the incoming character
          receivedChar = Serial.read();

          // Check if the received character is 'y' or 'n'
          if (receivedChar == 'y' || receivedChar == 'n') {
            if (receivedChar == 'y') {
              calibrated = true;
              Serial.println("Calibration Successful.");
            } else {
              // Calibration rejected
              calibrated = false;
              Serial.println("Calibration Rejected.");
            }
            receivedChar = Serial.read();
            receivedYN = true;
          }
        }
      }
    } else {
      Serial.println("HX711 not found.");
      delay(1000); // Wait before retrying
    }
    }


    // if done calibrating then print the current reading
    Serial.print("Current weight: ");
    Serial.print(scale.get_units(1));
    Serial.println(" g");

}

//calibration factor will be the (reading)/(known weight)
