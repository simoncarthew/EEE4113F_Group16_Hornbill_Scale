# Integrated Weight, Temperature, and Humidity Monitoring System for Southern Ground-Hornbill Conservation Research

## Background
Over the past few decades, the earth’s climate has been changing fast due to global warming. The
escalating threat of global warming poses the threat of extinction to sensitive species like the endangered
Southern Ground Hornbill. This has made scientists study the impact of these climatic changes on
these animals and how to improve their chances of survival to alleviate the threat of extinction. To
fully understand these impacts, scientists must monitor these animals and collect useful data that can
be analysed to provide insights on how to conserve them.

A team of researchers from UCT FitzPatrick Institute are spearheading the conservation of the Southern
Ground Hornbill through the Southern Ground Hornbill conservation program in Kruger National
Park, where they look into innovative ways of conserving these birds by monitoring their behaviours.
Specifically, Carrie, the stakeholder we engaged with, researches the impacts of heat stress on chick
growth and physiology, which are crucial for the birds’ survival. Carrie’s research relies heavily on
manual monitoring methods, which are labour-intensive and time-consuming and pose an injury risk to
the researchers. She also needs to understand how adult bird physiology influences chick development,
which requires the measurement of the adult bird’s weight. However, this is not possible currently as
she lacks a way to measure their weight to draw insights on this metric.

## Problem Statement
PhD student Carrie researches the effects of rising temperatures on the endangered southern be hornbill
chicks, and to fully understand its impact on them, she needs a non-invasive method to weigh and
identify individual adult birds, as their weight provides insight into how warmer temperatures can
affect both adults and chicks physiologies.

## Proposed Solution
The main idea is to have an artificial branch leveraging an individual bird’s weight onto a load cell
and a temperature and humidity sensor that monitors the external temperature to aid Carrie in her
research. Power generation, including solar panels, will also be implemented since the nest will only
be checked twice a month. A camera also captures the footage and will be matched to the collected
weight data to link the weights of the birds to the correct individuals. This solution will enable Carrie 
to collect critical weight data without disturbing the birds, significantly
advancing her research into the effects of global warming on Southern Ground Hornbills.

## Subsystem Breakdown
The solution is broken up into the following subsystems:

- **Power Subsystem**: Manages solar energy harvesting, battery charging and discharging and
implements power-saving protocols to ensure weeks of uninterrupted operation in remote locations.
- **Mechanical Subsystem**: Physical design of the scale that uses an artificial branch with integrated
load cells for precise weight measurement. It also provides the system with a weatherproof
housing protecting the system electronics.
- **Electronics and Controls Subsysetm**: Acquires and processes data from sensors, synchronises
camera image capture with weight measurements, manages data storage and transmission, and
controls system timing and power modes.
- **Dynamic Weight Analysis Subsystem**: Employs signal processing algorithms to filter out
noise and movement artifacts of the birds, stabilise weight readings, calculate accurate individual
bird weights, and provide error estimates for each measurement.
