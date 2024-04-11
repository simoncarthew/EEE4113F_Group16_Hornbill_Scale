// used to record the raw signal and send to laptop to be stored as text file

// imports
#include <Arduino.h>
#include "soc/rtc.h"
#include "HX711.h"
#include <vector>
#include <cmath>
#include <string>

// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 18;
const int LOADCELL_SCK_PIN = 5;

HX711 scale;

// calibration
bool calibrated;
bool weight_recieved;

void calibrate() {
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
}

// buffer to store the last second of data
const int bufferSize = 10; // Size of the buffer to store data
long buffer[bufferSize]; // Buffer to store data
int bufferIndex = 0; // Index to keep track of the buffer position

void wait_for_object(int thresh){
  long raw_thresh = (thresh * scale.get_scale()) + scale.get_offset(); // calculate the raw theshold value
  long val = scale.read();
  while (fabs(val) < fabs(raw_thresh)){
    // get the scale reading
    val = scale.read();
    
    // add reading to the buffer and ufdate buffer index
    buffer[bufferIndex] = val;
    bufferIndex = (bufferIndex + 1) % bufferSize;
  }
}

void get_signal(int thresh, std::vector<long>* vals){
  int max_times_below_thresh = 5;
  int no_times_below_thresh = 0;
  long raw_thresh = (thresh * scale.get_scale()) + scale.get_offset(); // calculate the raw theshold value
  long val;

  // keep grabing values while the signal has been above threshold for set no. times
  while (max_times_below_thresh > no_times_below_thresh){
    val = scale.read();
    (*vals).push_back(val);
    if(fabs(val) < fabs(raw_thresh)){no_times_below_thresh++;}else{no_times_below_thresh=0;} // increase no. times below if below and set to zero if not
  }
}

void next_test(){
  // initialise signal
  std::vector<long>* vals = new std::vector<long>;

  // inform user ready for next test
  Serial.println("Ready for next test.");
  
  // send the scale and offset vale
  Serial.println(scale.get_offset());
  Serial.println(scale.get_scale());

  // wait for the user to be ready
  wait_for_user();

  // wait for the next object to land
  Serial.println("Waiting for object.");
  wait_for_object(100);

  // get the objects weight signal
  Serial.println("Object detected.");
  get_signal(100,vals);
  Serial.println("Object left.");

  // push the second of data onto the front of vector
  for (int i = bufferSize - 1; i >= 0; i--) {
    (*vals).insert((*vals).begin(), buffer[(bufferIndex + i) % bufferSize]);
  }

  // send data to the laptop
  for(int i = 0; i < (*vals).size(); i++){
    Serial.println((*vals)[i]);
  }

  // confirm sent data
  Serial.println("Sent data.");
}

void wait_for_user() {
  Serial.println("Waiting for user. Enter \"start\" to begin.");
  while (Serial.available() == 0 || Serial.readStringUntil('\n') != "start") {
    // Do nothing until "start" command is received
  }
}

// set up the microcontroller
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
  wait_for_user();

  while (!calibrated){
    calibrate();
  }

  next_test();
}