/*
* Project: Southern Ground Hornbill Monitoring System
* Module: Weighing Scale Control Script (ESP32)
* 
* Description: This ESP32 code forms the core logic for the Southern Ground 
*              Hornbill weighing scale system in the EEE4113F Design Course at UCT. 
*              Its major tasks include:
*
*   1. Sensor Data Acquisition: Reads weight data from the scale, temperature 
*      and humidity from their respective sensors, and obtains timestamps from 
*      the RTC (Real-Time Clock).
*   2. Data Logging: Stores the collected sensor data and timestamps to an SD card.
*   3. Display Output: Presents system information on an OLED display. 
*   4. Weight-Triggered Recording: Detects the bird's presence on the scale and 
*      initiates video recording for identification purposes.
*
* Author: Travimadox Webb
* Date: 2024-04-18
* Version: 1.0
*/

//Implement sleeping modes next

#include <Arduino.h>
#include <HardwareSerial.h>
#include <vector> 
#include <String.h> 
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "FS.h"
#include "SD.h"
#include "RTClib.h"
#include "HX711.h"
#include <esp_sleep.h>

//_______________________________________________________________________________________________________________________________________________________________________________________
// Constants
#define I2C_ADDRESS 0x3c
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define CALIBRATION_FACTOR -229.75
#define PIN_SPI_CS 5
#define LOADCELL_DOUT_PIN 27//Will revert back to 16 if we encounter any issues
#define LOADCELL_SCK_PIN 4
#define WAKE_UP_PIN GPIO_NUM_33
#define UART2_TX 17
#define UART2_RX 16



//_______________________________________________________________________________________________________________________________________________________________________________________
// Sensor objects
HX711 scale;
RTC_DS3231 rtc;
Adafruit_BME280 bme;
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
HardwareSerial uart2(2); // Create a HardwareSerial object for UART2

//_______________________________________________________________________________________________________________________________________________________________________________________
// Variables
hw_timer_t *timer = NULL;//Timer for weather data

char days[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
float weight;
float temperature;
float humidity;


uint8_t HH;
uint8_t MM;
uint8_t SSS;
uint8_t DD;
uint8_t MMM;
uint16_t YY;
uint8_t DAY;
String timestamp;


String dataframe1;
String dataframe2;
File  dataweight;
File dataweather;
String header1 = "Timestamp,Weight";
String header2 = "Timestamp,Temperature,Humidity";

long background_scale_values[10]; //Circular buffer to store the last 10 readings
int background_reading_index = 0; //Index to keep track of the current reading in the circular buffer
std::vector<int> raw_scale_readings; //Vector to store the raw scale readings when the bird is on the scale


volatile bool bird_was_on_scale = false; //To ensure we only take photos in one iteration instead of in every iteration
//_______________________________________________________________________________________________________________________________________________________________________________________
//Thresholds for Control
long WEIGHT_THRESHOLD = -9200; //tHE rAW adc VALUE OF THE SCALE
volatile bool isWeatherready = false;
volatile bool shouldSleep = false;
const long WEATHER_INTERVAL = 60000000; //60s in microseconds



//_______________________________________________________________________________________________________________________________________________________________________________________
//Functions

//***************************************************************************************************************************************************************************************

/*
* This function reads the scale readings in the background when the bird is not on the scale
*/
void background_scale_readings() {
  //Read the scale readings in the background
  background_scale_values[background_reading_index] = scale.read();
  background_reading_index = (background_reading_index + 1) % 10;
}

//***************************************************************************************************************************************************************************************
/*
* This function checks if the bird is on the scale
*/
bool is_bird_on_scale() {
  //Check if the bird is on the scale
  if(scale.read() < WEIGHT_THRESHOLD) {
    return true;
  } else {
    return false;
  }
}



//***************************************************************************************************************************************************************************************

/*
* This function records the raw scale readings when the bird is on the scale
*/
void record_raw_scale_readings() {
  //Record the raw scale readings when the bird is on the scale
  raw_scale_readings.push_back(scale.read());
}



//***************************************************************************************************************************************************************************************
/*
* This function combines the background scale readings and the raw scale readings into one vector to be processed by dymanimic force analysis algorithm
* i.e [background_scale_readings(up until the background index), raw_scale_readings]
*/
std::vector<int> combine_scale_readings() {
  //Combine the background scale readings and the raw scale readings
  std::vector<int> combined_scale_readings;
  for(int i = 0; i < 10; i++) {
    combined_scale_readings.push_back(background_scale_values[i]);
  }
  for(int i = 0; i < raw_scale_readings.size(); i++) {
    combined_scale_readings.push_back(raw_scale_readings[i]);
  }
  return combined_scale_readings;
}



//***************************************************************************************************************************************************************************************
/*
* This function is the interrupt handler for the weather timer
*/
void IRAM_ATTR timerInterrupcion() {
 isWeatherready = true;
}




//***************************************************************************************************************************************************************************************
/*
* This function sets up the sensors and the serial communication(Serial is only for testing purposes)
*/
void setupSensors() {

  // Serial set up
  Serial.begin(115200);
  while (!Serial);

  //Hardware Serial set up
  uart2.begin(115200, SERIAL_8N1, UART2_RX, UART2_TX);
  while (!uart2);
  Serial.println("UART2 is ready");



  // Scale set up
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(CALIBRATION_FACTOR);
  scale.tare();

  // RTC setup
  if (! rtc.begin()) {
    Serial.println("Could not find RTC! Check circuit.");
    while (1);
  }
  rtc.adjust(DateTime(__DATE__, __TIME__));
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  // Temperature & Humidity Sensor setup
  bool status;
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }

  // OLED Setup
  display.begin();
  display.clearDisplay(); 
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  // SD card setup
  if (!SD.begin(PIN_SPI_CS)) {
    Serial.println("Error, SD Initialization Failed");
    return;
  }

  //Set up the weather timer
  timer = timerBegin(0, 80, true); // Timer 0, clock divider 80
  timerAttachInterrupt(timer, &timerInterrupcion, true); // Attach the interrupt handling function
  timerAlarmWrite(timer, WEATHER_INTERVAL, true); // Interrupt every 60 seconds
  timerAlarmEnable(timer); // Enable the alarm

 

  

}


//***************************************************************************************************************************************************************************************

/*
* This function sets up the files for the data to be written to
*/
void setupFiles() {
  // Weight file
  dataweight = SD.open("/DataWeight.txt", FILE_WRITE);
  if (dataweight) {
    dataweight.println(header1);
    dataweight.close();
    Serial.println("Success, data written to DataWeight.txt");
  } else {
    Serial.println("Error, couldn't not open DataWeight.txt");
  }

  // Weather data
  dataweather = SD.open("/DataWeather.txt", FILE_WRITE);
  if (dataweather) {
    dataweather.println(header2);
    dataweather.close();
    Serial.println("Success, data written to DataWeather.txt");
  } else {
    Serial.println("Error, couldn't not open DataWeather.txt");
  }
}

//***************************************************************************************************************************************************************************************

/*
* This function reads the data from the sensors and the RTC and creates a string to be written to the SD card
*/

String readtimeData() {
  DateTime now = rtc.now();
  HH = now.hour();
  MM = now.minute();
  SSS = now.second();
  DD = now.day();
  MMM = now.month();
  YY = now.year();
  DAY = now.dayOfTheWeek();

  // Combine the time stamp into one string
  timestamp = String(HH) + ":" + String(MM) + ":" + String(SSS) + " | " + String(DD) + "-" + String(MMM) + "-" + String(YY);
  return timestamp;
}

void readweatherData() {
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();

  // Combine the time stamp into one string
  timestamp = readtimeData();

  // Create dataframe to write
  dataframe2 =(timestamp + "," + String(temperature) + "," + String(humidity)); 
}

void readweightData() {
  weight = scale.get_units(5);
  // Combine the time stamp into one string
  timestamp = readtimeData();

  // Create dataframe to write
  dataframe1 = (timestamp + "," + String(weight));
}

float dynamic_weight_analysis(std::vector<int> combined_scale_readings, float calibration_factor) {
  //Dynamic force analysis algorithm
  //This function will be used to determine the weight of the bird on the scale
  //The algorithm will be implemented in the next version of the code
  float weight_placeholder = 123.56;
  //return 0;
  return weight_placeholder;
}

//***************************************************************************************************************************************************************************************


/*
*This function writes the data to the SD card
*/
void writeWeightData() {
  // Write data to weight file
  dataweight = SD.open("/DataWeight.txt", FILE_APPEND);
  if (dataweight) {
    dataweight.println(dataframe1);
    dataweight.close();
    Serial.println("Success, data written to DataWeight.txt");
  } else {
    Serial.println("Error, couldn't not open DataWeight.txt");
  }

 
}


void writeWeatherData() {
   // Write Weather data to weight file
  dataweather = SD.open("/DataWeather.txt", FILE_APPEND);
  if (dataweather) {
    dataweather.println(dataframe2);
    dataweather.close();
    Serial.println("Success, data written to DataWeather.txt");
  } else {
    Serial.println("Error, couldn't not open DataWeather.txt");
  }
}

//***************************************************************************************************************************************************************************************

/*
* This function displays the data on the OLED display
*Will be mostly useful for testing purposes and demonstration
*/
void displayData() {
  // Display data on the OLED
  display.clearDisplay();
  display.setCursor(0, 0);  // Position the cursor
  display.print("Weight: ");   
  display.println(weight); 

  display.setCursor(0, 10);
  display.print("Temp: ");
  display.println(temperature);

  display.setCursor(0, 20);
  display.print("Humidity: ");
  display.println(humidity);

  display.setCursor(0, 30);
  display.print("Timestamp:\n");  
  display.println(timestamp);

  display.setCursor(0, 50); 
  display.print("Day: ");
  display.println(days[DAY]);

  display.display(); // Update the OLED display
}

//***************************************************************************************************************************************************************************************
/*
*This fucntion sned the command to the esp32 cam to take a 5 pictures of the bird on the scale
*/
void takePictures(){
  //Command format: command,timestamp\n
  uart2.print("1,");
  String timestamp = readtimeData();
  uart2.println(timestamp);
  Serial.println("Command sent to ESP32 Cam to take pictures");



}



//***************************************************************************************************************************************************************************************

//_______________________________________________________________________________________________________________________________________________________________________________________
// Main Code
void setup() {
  // put your setup code here, to run once:
  setupSensors();
  setupFiles();

 
  
}

void loop() {

  //Purely for testing purposes. Read the ra adc value and print it to the serial monitor
  Serial.println(scale.read());

  // put your main code here, to run repeatedly:
  if(shouldSleep){
    Serial.println("Going to sleep now");
    esp_sleep_enable_ext0_wakeup(WAKE_UP_PIN, 0);
    esp_deep_sleep_start();
  }
  else{
    //If the bird is not on the scale, read the scale readings in the background
    if(!is_bird_on_scale()) {
      Serial.println("Bird is not on the scale");
      background_scale_readings();
    }
    else{
      Serial.println("Bird is on the scale and photos are being taken");
      takePictures();

      while(is_bird_on_scale()) {
      //Record the raw scale readings when the bird is on the scale
      Serial.println("Bird is on the scale and Data is being recorded");
      record_raw_scale_readings();
    }

    }

    //Need to think of a better way to implemnent tyhe below code//THink of firing interrupts

    //-----------------------------------------------------------------------
    //check if bird is on scale and take pictures
   /* if(is_bird_on_scale()){
      Serial.println("Bird is on the scale and photis are being taken");
      takePictures();
    }

    //Check if the bird is on the scale and record readings
    while(is_bird_on_scale()) {
      //Record the raw scale readings when the bird is on the scale
      Serial.println("Bird is on the scale");
      record_raw_scale_readings();
      //takePictures();
     // delay(100);
    }*/
     //-----------------------------------------------------------------------


    //Only proceed if raw_scale_readings is not empty
    if(!raw_scale_readings.empty()) {

      Serial.println("Processing weight data");
      //Combine the background scale readings and the raw scale readings
      std::vector<int> combined_scale_readings = combine_scale_readings();

      //get weight data
      weight = dynamic_weight_analysis(combined_scale_readings, CALIBRATION_FACTOR);

      //write weight data
      writeWeightData();

      //Printthe raw scale readings to the serial monitor as array
      Serial.print("Raw Scale Readings: [");
      for(int i = 0; i < combined_scale_readings.size(); i++) {
        Serial.print(combined_scale_readings[i]);
        if(i != combined_scale_readings.size() - 1) {
          Serial.print(", ");
        }
      }


      //clear the raw scale readings
      raw_scale_readings.clear();
    }


    //Collect weather data every 60s to implemented soon


    if(isWeatherready){

      Serial.println("Processing weather data");
      //get weather data
      readweatherData();
      writeWeatherData();
      isWeatherready = false; //will only be relevant once the update emchansim is implemented
    }

    //display data
    displayData();

    
    
    




  }
}

