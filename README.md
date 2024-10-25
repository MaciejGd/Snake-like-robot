# Snake-like-robot

## Robot project, and creation based on arm microprocessors

Here is code for controlling snake-like robot created by me. Snake is build of ten independent modules, every equipped with its own microcontroller for controlling movement. PCBs with microcontrollers has been designed by me, png of electrical connections and elements are placed in a repository in PCB-scheme.png file, layout of pcb is placed in PCB-layout.png. The first module is snake's head responsible for sending requsts to other elements. Snake is controlled by external device via bluetooth communication. For this purpose i use module HC-05 connected to microcontrollers UART. Upon receiving request from external bluetooth device snake's head is interpreting the request and sending request's for starting or stopping movement of other modules using rs485 electrical standard and my version of simplified modbus as protocol. Code responsible for master in connection (snake head) is placed under c_code/master and code for slave in c_code/slave dirs.

## Application for controlling wireless connection

Code in the file connection.py is responsible for graphical user interface, created with tkinter library and establishing bluetooth connection between snake's head and pc, using serial library. I am using personal computer with linux operating system, so rfcomm protocol is used to emulate serial ports for bluetooth communication.  To automate the process of binding mac adres of HC-05 module to rfcomm port I created bash script which is run from the python script using subprocess library. Simplyfing bash script is checking if module is in range and then binds it to the first available rfcomm port. Also serial communication in connection.py script is handled in another thread to prevent Gui and Serial to block another.

