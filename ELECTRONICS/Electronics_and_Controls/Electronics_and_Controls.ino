/*
*
* This ESP32 code is for the final project of the course "EEE4113F" Design Course at the University of Cape Town.
* The code is the control for bird weighing scale for the Southern Ground Hornbill.
* The code reads data from the scale, temperature and humidity sensor, and the real-time clock (RTC) and saves the data to an SD card.
* The code also displays the system on an OLED display.
* The code also records a video of the bird when the bird is on the scale and saves it to the SD card for bird identification.


* Author: Travimadox Webb
* Date: 2024-04-18
* Version: 1.0
*/

#include <Arduino.h>
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

//_______________________________________________________________________________________________________________________________________________________________________________________
// Constants
#define I2C_ADDRESS 0x3c
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define CALIBRATION_FACTOR -257.379
#define PIN_SPI_CS 5
#define LOADCELL_DOUT_PIN 16
#define LOADCELL_SCK_PIN 4

//_______________________________________________________________________________________________________________________________________________________________________________________
// Sensor objects
HX711 scale;
RTC_DS3231 rtc;
Adafruit_BME280 bme;
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//_______________________________________________________________________________________________________________________________________________________________________________________
// Variables
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

long background_scale_readings[10]; //Circular buffer to store the last 10 readings
int background_reading_index = 0; //Index to keep track of the current reading in the circular buffer
std::vector<int> raw_scale_readings; //Vector to store the raw scale readings when the bird is on the scale

//_______________________________________________________________________________________________________________________________________________________________________________________
//Thresholds for Control
long WEIGHT_THRESHOLD = 1000; //tHE rAW adc VALUE OF THE SCALE
int WEATHER_THRESHOLD = 30; //THE FREQUENCY OF THE WEATHER DATA IS RECORDING
int DAY_THRESHOLD = 1; //THE LIGHT INTESNITY TO SEND ESP32 TO DEEP SLEEP


//_______________________________________________________________________________________________________________________________________________________________________________________
//Functions

//***************************************************************************************************************************************************************************************

/*
* This function reads the scale readings in the background when the bird is not on the scale
*/
void background_scale_readings() {
  //Read the scale readings in the background
  background_scale_readings[background_reading_index] = scale.read();
  background_reading_index = (background_reading_index + 1) % 10;
}

//***************************************************************************************************************************************************************************************
/*
* This function checks if the bird is on the scale
*/
bool is_bird_on_scale() {
  //Check if the bird is on the scale
  if(scale.read() > WEIGHT_THRESHOLD) {
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
  for(int i = 0; i < background_reading_index; i++) {
    combined_scale_readings.push_back(background_scale_readings[i]);
  }
  for(int i = 0; i < raw_scale_readings.size(); i++) {
    combined_scale_readings.push_back(raw_scale_readings[i]);
  }
  return combined_scale_readings;
}



//***************************************************************************************************************************************************************************************

/*
* This function sets up the sensors and the serial communication(Serial is only for testing purposes)
*/
void setupSensors() {
  // Serial set up
  Serial.begin(9600);
  while (!Serial);

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
void readData() {
  // Read data from various sensors and RTC
  DateTime now = rtc.now();
  weight = scale.get_units(5);
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  HH = now.hour();
  MM = now.minute();
  SSS = now.second();
  DD = now.day();
  MMM = now.month();
  YY = now.year();
  DAY = now.dayOfTheWeek();

  // Combine the time stamp into one string
  timestamp = String(HH) + ":" + String(MM) + ":" + String(SSS) + " | " + String(DD) + "-" + String(MMM) + "-" + String(YY);

  // Create dataframe to write
  dataframe1 = (timestamp + "," + String(weight));
  dataframe2 =(timestamp + "," + String(temperature) + "," + String(humidity)); 
}

//***************************************************************************************************************************************************************************************


/*
*This function writes the data to the SD card
*/
void writeData() {
  // Write data to weight file
  dataweight = SD.open("/DataWeight.txt", FILE_APPEND);
  if (dataweight) {
    dataweight.println(dataframe1);
    dataweight.close();
    Serial.println("Success, data written to DataWeight.txt");
  } else {
    Serial.println("Error, couldn't not open DataWeight.txt");
  }

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



//_______________________________________________________________________________________________________________________________________________________________________________________

// Main code
void setup() {
  setupSensors();
  setupFiles();
}

void loop() {
  readData();
  displayData();
  writeData();
  delay(1000);
}