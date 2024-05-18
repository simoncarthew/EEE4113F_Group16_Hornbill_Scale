
# Power Subsystem Overview

The Power Subsystem serves as the power distribution center of the perch scale in design used to measure the weight of the Southern GroundHorn Bills.

### System Objectives:
* This system must able to harvest energy using Solar Panels to charge 12V-battery.
* This system must be able to charge using PWM controllers.
* This system must regulate the high 12V Voltage down to 5V/1A.
* This system must make use of protection schemes that disengage the rest of the system incases of undervoltage and overvoltage.
* This system must be able to tell battery health to aid State of Charge (SoC) and State of Health (SoH) Estimation.
 
### Subsystems of Power System
1. Energy Harvesting: Solar Panels harvests energy using solar radiation, cpnverts it to electrical energy to charge the battery.
2. PWM Controller: Generates PWM using NE555 Timer.
3. Regulation: Takes in 12V from the battery regulates it into 5V/1A and pushes it out to the loads.
4. Battery Status: Displays battery levels using zener-diode based LED circuit (Back-up)
5. Protection: Protects the ESP 32 and ESP32 CAM from reverse polarity, under-charging, undervoltage, overcharging and overvoltage

### Interfaces
This subusystem interfaces with two loads and that is:
1. ESP 32 MCU 
2. ESP 32 CAM 

