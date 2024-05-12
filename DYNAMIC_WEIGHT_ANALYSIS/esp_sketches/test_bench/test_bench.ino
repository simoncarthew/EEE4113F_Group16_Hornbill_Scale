// imports
#include <Arduino.h>
#include "soc/rtc.h"
#include "HX711.h"
#include "WeightProcessor.h"
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

// test variables
int min_bird_weight = 40;

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
int lastBufferIndex = 0;

void wait_for_object(int thresh){
  long raw_thresh = (thresh * scale.get_scale()) + scale.get_offset(); // calculate the raw theshold value
  long val = scale.read();
  while (fabs(val) < fabs(raw_thresh)){
    // get the scale reading
    val = scale.read();
    
    // add reading to the buffer and ufdate buffer index
    buffer[bufferIndex % bufferSize] = val;
    bufferIndex = (bufferIndex + 1);
  }
}

void get_signal(int thresh, std::vector<long>* vals){
  int max_times_below_thresh = 10;
  int no_times_below_thresh = 0;
  long raw_thresh = (thresh * scale.get_scale()) + scale.get_offset(); // calculate the raw theshold value
  long val;

  // keep grabing values while the signal has been above threshold for set no. times
  while (max_times_below_thresh > no_times_below_thresh){
    val = scale.read();
    (*vals).push_back(val);
    if(fabs(val) < fabs(raw_thresh)){ // increase no. times below if below and set to zero if not
      no_times_below_thresh++;
    }else{
      no_times_below_thresh=0;
    } 
  }
}

std::vector<long>* wait_and_get_signal(){
  // initialise signal
  std::vector<long>* vals = new std::vector<long>;

  // wait for the next object to land
  Serial.println("Waiting for object.");
  wait_for_object(min_bird_weight);

  // get the objects weight signal
  Serial.println("Object detected.");
  get_signal(min_bird_weight,vals);
  Serial.println("Object left.");

  // push the second of data onto the front of vector
  int no_taken = bufferSize;
  if (bufferIndex - lastBufferIndex < bufferSize){
    no_taken = bufferIndex - lastBufferIndex;
  }
  for (int i = no_taken - 1; i >= 0; i--) {
    (*vals).insert((*vals).begin(), buffer[((bufferIndex % bufferSize) + i) % bufferSize]);
    // buffer[(bufferIndex + i) % bufferSize] = scale.get_offset();
  }
  return vals;
}


int get_test_procedure() {
  int selectedProcedure = 0;

  Serial.println("Select test procedure 1-5:");
  
  while (true) {
    // Wait until data is available on Serial
    while (Serial.available() == 0) {
      delay(100); // Delay to avoid busy wait
    }
    
    // Read the input
    String input = Serial.readStringUntil('\n');
    
    
    // Convert input to integer
    selectedProcedure = input.toInt();
    
    // Check if the input is within the range
    if (selectedProcedure >= 1 && selectedProcedure <= 5) {
      break;
    } else {
      Serial.println("Invalid input. Please select test procedure 1-5:");
    }
  }
  
  return selectedProcedure;
}

void test_1(){
  std::vector<int> times = {2,30,60,120};

  for(int time : times){
    Serial.print("Place a 1kg weight on the scale while tapping and lifting it up with varying forces and frequency for approximately ");
    Serial.print(time);
    Serial.println(" seconds");
    std::vector<long>* vals = wait_and_get_signal();
    WeightProcessor processor = WeightProcessor(vals,scale.get_scale(), scale.get_offset(),10);
    std::pair<float,float> est = processor.estimateWeight();
    
    if (est.first >= 0){
      Serial.println("Estimation Success.");
      Serial.print("Estimated Weight: ");
      Serial.println(est.first, 3);
      Serial.print("Associated Error: ");
      Serial.println(est.second, 3);
    }else{
      Serial.print("Invalid signal. Validation Status:");
      Serial.println(est.first, 0);
    }
    lastBufferIndex = bufferIndex;
  }
}

void test_2(){
    Serial.println("Wait a second before placing a 1kg weight on the scale for more than 1.5 seconds.");
    
    std::vector<long>* vals = wait_and_get_signal();
    WeightProcessor processor = WeightProcessor(vals,scale.get_scale(), scale.get_offset(),10);
    std::pair<float,float> est = processor.estimateWeight();
    std::pair<int,int> bird_pres_int = processor.getUnpadInt();

    Serial.print("Total Time: ");
    Serial.println(vals->size() * 0.1, 2);
    Serial.print("Initial Non Bird Present Time: ");
    Serial.println((float)bird_pres_int.first * 0.1, 2);
    Serial.print("Ending Non Bird Present Time: ");
    Serial.println((float)vals->size() * 0.1 - (float)bird_pres_int.second * 0.1, 2);

    lastBufferIndex = bufferIndex;
}

void test_3(){
    Serial.println("Place a 1kg weight on the scale for more than 1.5 seconds.");
    
    std::vector<long>* vals = wait_and_get_signal();
    WeightProcessor processor = WeightProcessor(vals,scale.get_scale(), scale.get_offset(),10);

    Serial.print("Scale Value: ");
    Serial.println(scale.get_scale());
    Serial.print("Offset Value: ");
    Serial.println(scale.get_offset());

    
    std::vector<float>* weights = processor.getWeightSig();
    for (int i = 0; i < weights->size(); i++){
        Serial.print("Raw Value: ");
        Serial.print((*vals)[i]);
        Serial.print("\tWeight Value: ");
        Serial.println((*weights)[i],3);
        delay(50);
    }

    lastBufferIndex = bufferIndex;
}

void test_4(){
    Serial.println("Place a 1kg weight on the scale for 10 minutes.");
    
    std::vector<long>* vals = wait_and_get_signal();
    WeightProcessor processor = WeightProcessor(vals,scale.get_scale(), scale.get_offset(),10);
    std::pair<float,float> est = processor.estimateWeight();
    std::vector<float>* weights = processor.getWeightSig();

    Serial.print("Weight Vector length: ");
    Serial.println(weights->size());
    Serial.print("Weight Vector size: ");
    Serial.print(((float)weights->size() * 32.0f) / (8.0f * 1024.0f),3);
    Serial.println("kb");

    Serial.println("Estimation Success.");
    Serial.print("Estimated Weight: ");
    Serial.println(est.first, 3);
    Serial.print("Associated Error: ");
    Serial.println(est.second, 3);

    
    lastBufferIndex = bufferIndex;
}

void test_5(){
    // invalid signal length
    Serial.println("Place a 1kg weight on the scale for less than 1.5 seconds ");
    std::vector<long>* vals = wait_and_get_signal();
    WeightProcessor processor = WeightProcessor(vals,scale.get_scale(), scale.get_offset(),10);
    int val = processor.validateSignal(500,1500,0.4,1.5,processor.getWeightSig());
    Serial.print("Validation status: ");
    Serial.println(-1 * val);
    lastBufferIndex = bufferIndex;

    // invalid minimum weight
    Serial.println("Place a weight less than the minimum weight of 0.5kg on the scale for more than 1.5 seconds. ");
    vals = wait_and_get_signal();
    processor = WeightProcessor(vals,scale.get_scale(), scale.get_offset(),10);
    val = processor.validateSignal(500,1500,0.4,1.5,processor.getWeightSig());
    Serial.print("Validation status: ");
    Serial.println(-1 * val);
    lastBufferIndex = bufferIndex;

    // invalid minimum weight
    Serial.println("Place a weight more than the maximum weight of 1.5kg on the scale for more than 1.5 seconds. ");
    vals = wait_and_get_signal();
    processor = WeightProcessor(vals,scale.get_scale(), scale.get_offset(),10);
    val = processor.validateSignal(500,1500,0.4,1.5,processor.getWeightSig());
    Serial.print("Validation status: ");
    Serial.println(-1 * val);
    lastBufferIndex = bufferIndex;

    // invalid minimum weight
    Serial.println("Place a weight between 0.5 and 1.5 kg’s on the scale for more than 1.5 seconds.");
    vals = wait_and_get_signal();
    processor = WeightProcessor(vals,scale.get_scale(), scale.get_offset(),10);
    val = processor.validateSignal(500,1500,0.4,1.5,processor.getWeightSig());
    Serial.print("Validation status: ");
    Serial.println(-1 * val);
    lastBufferIndex = bufferIndex;
    
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
  delay(1000);

  while (!calibrated){
    calibrate();
  }

  int test = get_test_procedure();

  if (test == 1){
    test_1();
  } else if (test == 2){
    test_2();
  }else if (test == 3){
    test_3();
  }else if (test == 4){
    test_4();
  }else if (test == 5){
    test_5();
  }

  lastBufferIndex = bufferIndex;
}