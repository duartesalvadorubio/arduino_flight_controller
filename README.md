# arduino_flight_controller
 
This has been my bachelor's degree final project in Electronics and Automation Engineering, called *Design, assembly and programming of an unmanned aircraft (drone)* and presented in September 2021. 
The project consisted of the construction of a quadcopter drone, both the hardware assembly and the programming of the flight controller, made with an Arduino Mega microcontroller. 

This repository presents the code used in this flight controller, structured in several files in which the code for the different functionalities of the drone is separated. The main functionality is the control of the aircraft itself by using PID controllers and a radio control system. In adittion to this, the flight controller also implements a two-axis camera stabiliser and a remote telemetry system. 

The drone had real-time imaging too, but this was an autonomous system and does not depend on the code of the flight controller so there is nothing to be found relating to it.

**NOTE:** It is worth mentioning that **the project was carried out entirely in Galician language** and therefore both the function names and the code comments are in this idiom. 

## 1. Code structure

The code is structured in different C++ source files with their respective headers. Each of these files contains the definition of functions that implement each functionality of the drone. 



**Source code files:**

- **arduino_flight_controller.ino**: This is the main file, where all the functions defined in the rest of the files are used.
lecturaMPU6050: It defines the functions in charge of handling the accelerometer and gyroscope module to obtain the acceleration and angular velocity measurements.

- **lecturaMPU6050**: The necessary functions are defined to operate the MPU6050 accelerometer-gyroscope through the I2C protocol, taking acceleration and angular velocity measurements.

- **lecturaRC.cpp**: This contains the functions in charge of taking the PWM signals received from the radio receiver, which will be the commands commanded by the drone pilot.

- **LED.cpp**: Responsible for managing the lighting of the drone.

- **PID.cpp**: Implements the different discretised PIDs that will act according to the selected flight mode to control the motors according to the commands given by the pilot.

- **sinaisPWM.cpp**: This module implements the necessary functions to generate the PWM signals that will drive the ESCs of the brushless motors according to the action commanded by the PIDs.

- **telemetria.cpp**: Defines the necessary functions to send and receive serial messages through the HC-12 radio frequency module.
Xeral: Implements some extra functionalities such as displaying parameters through the serial port or measuring the battery voltage.



## 2. Project abstract shown in the report

The project consists of the construction and design of a quadricopter drone for aerial filming and the
programming an Arduino-based flight controller, focusing mainly on the
assembly of all electronic components, programming of the controller and others
drone features.
With this, the interest of the work lies in deepening in a practical way the knowledge about
microcontrollers, programming and integration of elements in a real system.
It covers the step-by-step construction of the aircraft, the installation of all components
electronics (inertial measurement unit, speed variators, radio control receiver, etc.)
in an electronic circuit soldered on a prototyping PCB, which together with an Arduino Mega form
the flight controller.
On the other hand, design and 3D printing are used to make certain parts and elements
of support for apparatus components, including an articulated stabilizer support for carrying
a video camera, which allows you to add knowledge in this area.
A First Person View (FPV) system is also installed to help
in the piloting of the drone and the development of a telemetry system (which allows to know in
real time certain flight parameters) with radio transmission-reception modules. For this one
system is designed an application with a graphical interface that will be executed on the user's PC for
view the data.

## 3. Illustrative drone images

Drone views

![Xeral](https://user-images.githubusercontent.com/101353583/215902240-5e05daa2-620c-4b7c-8ce5-d96797773eac.jpg)

![Alzado](https://user-images.githubusercontent.com/101353583/215902280-33d5c212-fe97-4939-8896-d5666d73fd2e.jpg)

![Planta](https://user-images.githubusercontent.com/101353583/215902299-e20ac4bc-6829-4396-a759-b1a56a036764.jpg)

Flight controller

![Shield _editada](https://user-images.githubusercontent.com/101353583/215902696-1c79593b-9084-4b5f-8a67-5b4f2af4ee8f.jpg)


