# PHYSICAL SCALE
## Overview
The problem analysis identified the need for a non-invasive method to weigh individual adult Southern
Ground-Hornbill birds, providing an insight into how warmer temperatures can affect the their
physiologies. This could be achieved by leveraging an individual bird’s weight onto a load cell, as well
as making use of a temperature sensor to monitor the external temperature and a camera trap to
capture footage of the birds. This subsystem includes the housing of these components, as well as the integration 
of the load cell. The design was created with the researchers at the APNR Southern Ground-Hornbill Research and
Conversation Project and the FitzPatrick Institute of African Ornithology, as the intended users.

## Objectives 
- **Non Invasiveness**: The mechanical scale must blend in with the natural environment and be
designed in a way which the birds voluntarily perch on it.
- **Durability**: The design must be durable, weatherproof, and able to withstand the weight
of the birds and unpredictable weather conditions.
- **Measurements**: The scale must accurately measure the weight of individual adult birds.
- **Easy Setup and Transportation**: The user should be able to setup the scale with minimal effort and it
must be easily transportable. 

## Load Cell Selection
The optimal selection for this design is the Single Point load cell, as it is affordable and allows
placement variability. This is crucial because there is no way of knowing how or where about on the design the
birds will sit each time. Since the birds have a weight of around 2-5kg, a robust design is required
with this load cell being ideal as it is used for lighter loads and is able to detect small changes in weight. Initially,
a 10kg load cell was chosen. However, since the maximum weight of the Southern Ground Hornbill is 5kg, if two
birds of this weight were to sit on the scale at the same time, an inaccurate reading would be produced. This is
due to the load cell only being able to withstand a weight of 10kg. Thus, in order to account for this situation
and ensure accurate weight measurements, a 20kg load cell was chosen instead.

## Load Cell Topology
After assessing various options, the load cell placement which was chosen involves strategically placing two load cells
on either side of the bearing plate. This configuration ensures symmetry and balance of the overall structure of the scale.
The difference between having two load cells versus one load cell is that one pivot point would most likely result
in inaccurate weight measurements due to a specific bird landing further away from the load cell in question. Since this design makes use of two load cells, they can be connected in parallel to produce an average voltage
difference output, which increases the accuracy of the weight data that will be collected. In addition, this
placement allows for a longer perching area with increased stability, which is better suited for the type of birds
being considered.

## Load Cell Integration
The integration of the load cell with the ESP32 microcontroller is an important aspect of the design to ensure
that accurate weight readings are collected. As mentioned previously, the load cell outputs a small voltage
difference, so an amplifier is needed to interface with the microcontroller. This can be achieved through the
use of an instrumentation amplifier connecting to the ADC of the ESP32 or the HX711, which is a dedicated
precision 24-bit ADC designed for this purpose. After comparing the two options, it is
evident that the HX711 is a better design choice. Therefore, it will be used instead of the amplifer circuit
designed. The two load cells used in this design will be connected in parallel to the HX711.

## Final Design
Based on previous iterations, the final design was created on OnShape and the measurements redefined. The mechanical scale was designed and assembled in parts, consisting of the main outer box, the inner waterproof box to house the electronics and a smaller box to house the camera module and temperature sensor. The load cell is placed between the outer box lid
and the Plexiglass lid leaving enough room to allow for deformation. Additionally, there are two holes on the small box and the main box which match up. This is to allow for the wiring to pass through to the rest of the electronics being housed in the Plexiglass box. There is also an extra hole on the inner box lid to allow for the wires of the load cell to enter the electronics housing compartment. Passive ventilation was incorporated, as well as slots to strap the structure to a near-horizontal branch.

## Testing
Testing was done including strength testing, user interface, accuracy of measurements and whether the deisgn is weatherproof. The following was achieved: 
- **Ingress Protection Rating**: An IP55 Rating was achieved.
- **Durability**: Design can withstand the maximum weight of the birds of 5kg.
- **User-Friendly**: Takes about 10 minutes to setup, making it easy to setup and transport for the user.
