# DYNAMIC WEIGHT ANALYSIS
## Overview
The dynamic weight analysis system is responsible for aquiring a static weight estimation of the measured bird from the digital force signal outputted by the load cell.

## Execution Environment
The system is implemented as a c++ class, "WeightProcessor", that can be used by any computing environment capable of instantiating a c++ class as a class object. The object manages the load cells output signal and its respective calculated signals as dynamically allocated arrays of type double. By way of a member function call, one can retrieve the weight estimation, associated error and success status of the calculation.

## Subsubsystems
Each subsystem is implemented by way of member function calls:
- **Load Cell Signal to Force**: uses the calibration factor to convert the digital signal recieved from the ADC to a force signal
- **Force Conversion**: converts the force read at the load cell to the vertical force exerted by the bird
- **Weight Estimation**: analyses signal and chooses the relevant method to estimate the birds weight from the downward force exerted by the bird
- **Weight Error**: produces an error associated with the estimated weight
- **Success Status**: validates that the provided load cell signal and final weight estimation meets certain requirements (e.g. adequate length/max weight/min weight)

## Interfaces

### Electronics & Control:

- **Input**: EC must provide the system with the raw digital signal retrieved from the ADC and its calibration factor
- **Output**: System must return a weight estimation, associated error and success status
- **Communicating**:
    The digital signal from the load cell and its calivration factor are communicated to the system by creating a WeightProcessor object and inputting them as input parameters to the classes constructor. The weight estimation, associated error and success status will be communicated back to the EC system by calling the "estimateWeight" member function that returns said values in appropriate types.

### Physical Scale:

- **Force Conversion**: physical system must be modeled using static body force equations to convert the force applied to the load cell to the downward force being exerted by the bird which depends on the scales physical structure
