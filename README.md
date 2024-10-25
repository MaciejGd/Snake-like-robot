# Snake-like-robot

## Operating principle

This project includes, creation of the snake-like robot from scratch.
Robot is microcontroller based. Microcontroller boards used, are my design, electrical scheme and layout of PCB are placed under `pictures` directory. Robot consists of ten independent modules, every equipped with its own microcontroller board responsible for handling movement and communication. The first module is snake's head responsible for sending requests to other elements. Rest of elements simply interpret acquired information and transfer it into a robot movement. Three modes of snake movement are available: glinding, sidewinding to the left and sidewinding to the right.

## Communication  

Communication between robot and external device has to be established to let control the robot's movement. For this purpose I choose bluetooth communication using HC-05 bluetooth module. I wrote python application for Linux based systems, offering simple UI for robot control and connection's parameters configuration. Communication between robot's modules has been implemented using rs485 electrical standard with custom communication protocol (inspired by modbus).

## Microcontroller's code

I programmed microcontrollers using `C` programming language. Code for both master and slave (head of snake, and rest of the modules) has been placed under `c_code` directory of current repository. 

## Application for controlling wireless connection

Code in the file connection.py is responsible for graphical user interface and establishing bluetooth connection between snake's head and pc. To automate the process of binding mac adres of HC-05 module to rfcomm port I created bash script `connect.sh` which is run from the python script. Script `disconnect.sh` is simple automation script for freeing connection on port 0 which is in most cases used for connecting to robot. 

