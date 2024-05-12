#include <Arduino.h>
#include <esp_sleep.h>

#define WAKE_UP_PIN GPIO_NUM_33
#define SLEEP_TIME 3600e6  // Sleep for 1 hour

volatile bool shouldSleep = false;

// This structure will be stored in the RTC memory
typedef struct {
  uint32_t counter;
} wake_up_counter_t;

// Initialize the structure
RTC_DATA_ATTR wake_up_counter_t wake_up_counter = {0};

bool daylightSensor() {
  // Read the value of the wake-up pin
  return digitalRead(WAKE_UP_PIN) == HIGH;
}

void setup(){
  Serial.begin(115200);
  pinMode(WAKE_UP_PIN, INPUT_PULLDOWN);

  // Increase the counter and print its value
  wake_up_counter.counter++;
  Serial.print("Woke up. This is wake-up number: ");
  Serial.println(wake_up_counter.counter);
}

void loop(){
  shouldSleep = daylightSensor();

  if (shouldSleep) {
    Serial.println("Going to sleep now. Goodnight!");
    esp_sleep_enable_timer_wakeup(SLEEP_TIME);
    esp_deep_sleep_start();
  }
  else {
    Serial.println("System is awake. Ready to Monitor these birds!");
    delay(1000); 
  }
}