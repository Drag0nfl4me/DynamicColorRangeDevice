# DynamicColorRangeDevice
Capstone project for CNM's Internet of Things boot-camp. Use a camera to take two pictures. One being unaltered the other being taken after a bright colored light is turned on. I would then compare every single pixel from these images and find the change in color. This change in color would be transferred to a new image.

## Parts used in this project 

* ESP32 Camer Module
* Particle Argon
* Darlington High Power Transistor
* 3W RGB 2.2-3.2V Color High Power LED

## Softwares

Visual Studio Code
* Holds the LED code and logic to send a command to an ESP32

Arduino IDE
* Holds the camera and MQTT (Adafruit) publish code

Processing
* Responsible for combining images relative to RGB value differences

### Learning Objectives

* Use technology in a way I haven't seen before
* Learn how to use a few new components

#### Caden Gamache, 03/10/2023
