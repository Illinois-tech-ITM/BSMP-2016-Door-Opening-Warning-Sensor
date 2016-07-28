# BSMP-2016-Door-Opening-Warning-Sensor

This repo contains the code and schematics for creating an early warning system for people walking in creating an early warning system for doors opening into hallways

---------------------
### Participants
* Gabriel Filipe Botelho Barbosa ([@bestknighter](@bestknighter))
* Guilherme de Castro ([@guidecastro](@guidecastro))
* Luiz Gustavo de Marchi Brito
* Matheus Braz Duque
* Rodrigo Ferreira Abdulmassih

---------------------
### Instructions
If you want to reproduce this project, follow the steps below.

1. Download an Arduino IDE of your preference (we used their [official one](http://www.arduino.cc))
2. Build the project as in the [Fritzing diagram](/Documents/Diagram.png). _If you don't have a solar panel, you can ignore the solar panel charger and simply power it through the micro-usb._
3. Open the [main code](/Arduino_Micro_Code/Arduino_Micro_Code.ino) in the IDE.
4. Build and upload it. __Make sure to set the correct arduino model before uploading.__

You can tweak with the constants declared as Macros to calibrate the project for your use. The values in this repo is the one that worked best with us.

##### _Troubleshooting_

Problem | Solution | Reason
------ | ------- | --------
I uploaded the code and it is running excruciatingly slow/not running at all | If you're running with an external power, build the production version (comment all DEBUG defines). If you're powering with PC usb, open a serial monitor | This happens because the arduino waits the serial port to be read, halting the program until done so or until timeout (which takes usually too long)
Buzzer sounds but LEDs don't/LEDs lights up but buzzer don't | Check your cables to see if they are firmly attached, or change them, and plugged in to the right port described in the Macro areas | The signal may not be reaching the component due to a broken cable, loose cable or wrong port
Alarm does not behave as expected/is weird. | Callibrate your system by building the DEBUG version (uncomment just the `#define DEBUG`) and changing the constants. | Your case/scenario may be different from ours. Activating the debug mode allows you to understand what is happening and then change the constants value to what may fit best.

##### _Known Issues_
* When the distance with no one in front of the sensor is smaller than 2.5m and then someone enters (making the detected distance quickly reduce) is understood as someone walking towards the door, thus, triggering the alarm.
* The solar panel by itself can power the system and when the battery is charged can also power the system for a long time. But when the battery is plugged to the solar panel and powering the system, it stops working quickly. Using a power bank to power the system also works for a long time.