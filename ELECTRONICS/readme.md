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

- **Load Cell Sensor**: Measures nest weight. Includes details for calibration and signal amplification.
- **Temperature Sensor**: Monitors external nest temperature.
- **Humidity Sensor**: Records humidity levels around the nest.
- **Camera/Visual Sensor**: Captures short video clips for researcher's bird identification.
- **Daylight Sensor**: Triggers power saving/sleep modes based on light availability.
- **Rationale**: Power conservation is essential for extended field deployment.
  

### Control

- **Sensor Activation/Deactivation**: Manages sensor operation based on pre-defined thresholds or time intervals.
- **Data Management Control and Flow**: Regulates data processing and storage operations.

## Interfaces
Still in progress

### Power Interface:

- **Source**: Power subsystem
- **Voltage & Current Specifications**: Detail minimum and maximum requirements.(Detailed one Coming soon)
- **Connection**: Power pins, ADC pins for battery monitoring (Detailed one coming soon)


### Dynamic Weight Analysis Subsystem:

- **Output**: Load cell signal from amplifier
- **Input**: Processed weight value (with accuracy)
- **Communication**: Function calls


### Mechanical Scale:

- **Space Requirements**: Specific dimensions.(Detailed one coming soon)
- **Damage Protection**: (Detailed one coming soon).