# Electronics & Control Subsystem
## Overview
The Electronics & Control Subsystem is the central data acquisition, processing, and management unit within the larger automatic weighing system for Southern Ground Hornbill nests. Its core functionalities include:

- **Data Acquisition**: Retrieval of data from various sensors (temperature, humidity, load cells) in real-time.
- **Data Processing**
   - Sensor calibration
   - Raw data to meaningful data conversion (temperature, weight, humidity)
- **Data Storage**: Organized saving of processed data for later analysis.
- **Dynamic Weight Analysis**: Provides input to the Dynamic Weight Analysis Subsystem.
- **System Control**: Coordination of power management, sensor activation, and data flow based on environmental conditions and requirements.


## Subsubsystems

### Data Management

- **Data Processing**: Calibrates and converts raw sensor readings.
- **Data Reception and Transmission**: Handles incoming data streams and outgoing transmissions.
- **Data Storage**: Manages storage on an SD card or other medium, including file naming conventions and error handling.
- **Data Visualization**: Provides an interactive website interface for data viewing and retrieval.
- **Rationale**: A user-friendly interface streamlines data access for the researcher.


### Sensors and Sensor Integration

- **Load Cell Sensor**: Measures birds weight. Includes details for calibration and signal amplification.
- **Temperature Sensor**: Monitors external nest temperature.
- **Humidity Sensor**: Records humidity levels around the nest.
- **Camera/Visual Sensor**: Captures short video clips for researcher's bird identification.
- **Daylight Sensor**: Triggers power saving/sleep modes based on light availability.
- **Rationale**: Power conservation is essential for extended field deployment.
  

### Control

- **Sensor Activation/Deactivation**: Manages sensor operation based on pre-defined thresholds or time intervals.
- **Data Management Control and Flow**: Regulates data processing and storage operations.

## Interfaces
First Version

| Interface | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           | Pins/Output                                                                                    |
| --------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------- |
| I001      | Power Subsystem Interface:<br><br>\- Supplies power to the electronics subsystem<br>\- Supply voltage range: 3V to 5V DC<br>\- Supply current: Up to 800mA<br><br>The power subsystem connects to the electronics through the power pins of the microcontroller unit (MCU) and sensor modules. An analog-to-digital converter (ADC) pin on the MCU monitors battery level and health.                                                                                                                                                                                                                 | \- Power pins of MCU and sensors<br><br>\- ADC pin(s) for battery monitoring                   |
| I002      | Dynamic Force Signal Analysis Subsystem Interface:<br><br>\- The HX711 load cell amplifier converts the analog load cell signal to a 24-bit digital value<br><br>\- This digital value is passed to the Dynamic Force Signal Analysis Subsystem<br><br>\- The subsystem applies filtering, noise compensation, and calibration to calculate the applied force/weight and associated accuracy<br><br>\- The calculated weight and accuracy are passed back to the electronics subsystem<br><br>The interface uses function calls like SendLoadCellData(value) and ReceiveWeightData(weight, accuracy). | Custom Function Calls/Library Integration                                                      |
| I003      | Mechanical Subsystem Interface<br><br>\- The electronics components require a placement space within the mechanical subsystem<br><br><br>\- Rationale: protect the components from potential damage by the Southern Ground Hornbill                                                                                                                                                                                                                                                                                                                                                                   | Load Cell Placement<br><br>Wires from Load Cell to The HX711 Amplifier<br><br>Camera Placement |
