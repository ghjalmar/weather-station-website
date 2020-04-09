# weather-station-website

﻿In the spring of 2018 I got the idea of doing a project of my own to get a more hands-on experience with programming and testing my abilities on a project outside of school/ work. I decided on doing a simple project but do it on a wider scale than I am used to. So instead of focusing on one topic I wanted to do a project that consisted of different parts and combine them in a single project. Therefore I came up with the weather-station-website. 

Code for my project on measuring the 'weather' inside my home, plotting the data and making it accessible on a website.

The project consists of three different main parts:
* A micro-controller (an arduino clone) which measures temperature, pressure and humidity using an external sensor. The micro-controller then transmits those sensor readings wirelessly to a RaspberryPi.
* RaspberryPi which receives the sensor readings and stores them in a SQL database. The Raspberry periodically plots the latest sensor readings and saves the plots.
* Website that displays the latest plots of the sensor readings. The website was hosted on the RaspberryPi as well and the traffic was routed through my router at home.

About the code:
* The code for the micro-controller is written using the Arduino editor and compiler. (Arduino_code.ino)
* The program on the Raspberry which receives the sensor readings is written in C++ and uses TMRh20’s library. (receive_log_data_reduced.cpp)
* I used SQLite3 for managing the database of the sensor readings, due to it’s simplicity.
* The plots are done in Python using Matplotlib.
* The website used to be hosted on my RaspberryPi and the traffic was routed through my router at home. The website is currently down. 

Pictures of the set up: 
![The RaspberryPi and the attached nRF module.](/pictures/rpi.png)
![The micro-controller, attached are nRF module and the sensor.](/pictures/snsr.png)
