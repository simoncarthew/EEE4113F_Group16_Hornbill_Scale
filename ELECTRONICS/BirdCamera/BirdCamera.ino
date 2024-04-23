/*
* Project: Southern Ground Hornbill Monitoring System
* Module: Camera Control Script (ESP32)
* 
* Description: This ESP32 code manages the camera used in the Southern Ground 
*              Hornbill monitoring system for the EEE4113F Design Course at UCT. 
*              Its primary functions include:
*
*   1. Scheduled Camera Activation: Wakes the camera at predetermined intervals 
*      for routine video recordings of the weighing scale area.
*   2. Weight-Triggered Recording: Initiates video recording when the bird is 
*      detected on the scale, capturing footage for identification.
*   3. Data Management: Saves videos and timestamps to the SD card for analysis.
*
* Author: Travimadox Webb
* Date: 2024-04-18
* Version: 1.0
*/

//Adaption of Rui Santos to be implemetented to make it record videos instead of taking pictures of a predefoned time length

#include <Arduino.h>
#include "esp_camera.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include "time.h"

// Pin definition for CAMERA_MODEL_AI_THINKER
// Change pin definition if you're using another ESP32 camera module
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

int pictureNumber = 0; // Number of the picture taken

// Stores the camera configuration parameters
camera_config_t config;


// Initializes the camera
void configInitCamera(){
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; //YUV422,GRAYSCALE,RGB565,JPEG
  config.grab_mode = CAMERA_GRAB_LATEST;


  // Select lower framesize if the camera doesn't support PSRAM
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10; //0-63 lower number means higher quality
    config.fb_count = 1;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Initialize the Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
}

// Get the picture filename based on the current ime
String getPictureFilename(){

  String filename = "/picture_" + String(pictureNumber) +".jpg";
  pictureNumber++;
  return filename; 
}

// Initialize the micro SD card
void initMicroSDCard(){
  // Start Micro SD card
  Serial.println("Starting SD Card");
  if(!SD_MMC.begin()){
    Serial.println("SD Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    return;
  }
}

// Take photo and save to microSD card
void takeSavePhoto(){
  // Take Picture with Camera
  camera_fb_t * fb = esp_camera_fb_get();

  //Uncomment the following lines if you're getting old pictures
  //esp_camera_fb_return(fb); // dispose the buffered image
  //fb = NULL; // reset to capture errors
  //fb = esp_camera_fb_get();

  if(!fb) {
    Serial.println("Camera capture failed");
    delay(1000);
    ESP.restart();
  }


  // Path where new picture will be saved in SD Card
  String path = getPictureFilename();
  Serial.printf("Picture file name: %s\n", path.c_str());

  // Save picture to microSD card
  fs::FS &fs = SD_MMC; 
  File file = fs.open(path.c_str(),FILE_WRITE);
  if(!file){
    Serial.printf("Failed to open file in writing mode");
  } 
  else {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved: %s\n", path.c_str());
  }
  file.close();
  esp_camera_fb_return(fb); 
}


void takemultiplePhotos(int n){
  for(int i = 0; i < n; i++){
    takeSavePhoto();
    //delay(1000);
  }
}


void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // disable brownout detector


  Serial.begin(115200);
  delay(2000);

  // Initialize the camera  
  Serial.print("Initializing the camera module...");
  configInitCamera();
  Serial.println("Ok!");
  // Initialize MicroSD
  Serial.print("Initializing the MicroSD card module... ");
  initMicroSDCard();
}

/*void loop() {    
  // Take and Save Photo
  takeSavePhoto();
  delay(10000);
}*/

void loop() {
  if (Serial.available()) { // Check if data is available to read
    String command = Serial.readStringUntil('\n'); // Read the incoming data until newline character

    if (command == "take_picture") { // Replace "take_picture" with your desired command
      //takeSavePhoto();
      takemultiplePhotos(5);
       
    }
  }
}