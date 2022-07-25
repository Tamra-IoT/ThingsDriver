# ThingsDriver

## Unified Interoperable Driver for IoT Nodes (UIDI)


![Build Status](https://travis-ci.org/joemccann/dillinger.svg?branch=master)

## Contents
 1. [Why ThingsDriver?](#1-why-thingsdriver)
 2. [Quick start](#2-quick-start)
    - [Required Part](#required-part)
    - [Download ThingsDriver repository](#download-thingsdriver-repository)
    - [Install ThingsDriver Firmware on Arduino UNO Board](#install-thingsdriver-firmware-on-arduino-uno-board)
    - [Install ThingsDriver Firmware on NodeMCU Board](#install-thingsdriver-firmware-on-arduino-uno-board)
    - [Connect The Two Boards as Shown Below](#connect-the-two-boards-as-shown-below)
 3. [Citations](#3-citations)
 4. [About Us](#4-about-us)
 5. [Copyright and Licenses](#5-copyright-and-licenses)

## 1. Why ThingsDriver
ThingsDriver is a new methodology in how we can govern IoT devices. When we connect a device such as a printer to the local personal computer (PC), the PC requires installing the printer driver to get the printer configurations. ThingsDriver adopts the same concept, where IoT devices in the thingsDriver architecture are extendable from the cloud, and the cloud needs the IoT devices driver to govern them. Looking at IoT devices as extendable components from the cloud can pave the way for a collaborative IoT environment.

## 2. Quick Start
 ### Required Part
 We developed this repository as a proof of concept to the Thingsdriver idea. The demo version runs over [NodeMCU](https://www.nodemcu.com/index_en.html) and [Arduino UNO](https://docs.arduino.cc/hardware/uno-rev3) boards as an IoT node in addition to [HiveMQ](https://www.hivemq.com/) cloud, a public MQTT broker, as the cloud side.
 1. [NodeMCU](https://www.nodemcu.com/index_en.html) 
 2. [Arduino UNO](https://docs.arduino.cc/hardware/uno-rev3) 
 3. Free broker on [HiveMQ](https://www.hivemq.com/)
 4. Some wires
 5. 5V-2A adapter
 6. USB 2.0 Cable Type A/B
 7. USB to Micro USB Cable wire
 ### Download ThingsDriver repository 
 Use the command below to download the demo code using the command line interface, or click [here](https://github.com/Tamra-IoT/ThingsDriver/archive/refs/heads/main.zip) to download the demo code as a zip file.
 
 ```
git clone https://github.com/Tamra-IoT/ThingsDriver.git
```
 ### Install ThingsDriver Firmware on Arduino UNO Board 
 (Peripherals controller Firmware)
 1. Using the Arduino IDE, open the AVR code.
 2. Connect your Arduino board using the USB cable to your computer.
 3. Choose the right board and terminal from the tools menu from the menu bar. For the board, choose UNO and for the terminal, select the one that appears in your list COM3/COMx, and sometimes the list shows the connected board names.
 4. Upload the code to the connected board using the right arrow symbol on the up-left corner.
 5. Don't disconnect the board until you see "done uploading" down on the green bar.  
 
 ### Install ThingsDriver Firmware on NodeMCU Board. 
 (Gateway Firmware)
 
 Follow the same step that you did to upload the code on the Arduino UNO board, but select the right board name, NodMCU, and the right port that will appear when you connect the NodMCU board to your computer.
 
 Hint: If you did not use the Arduino IDE to upload codes on the NodMCU before, you have to add the NodMCU configuration on the Arduino IDE first. Follow this link [here](https://create.arduino.cc/projecthub/electropeak/getting-started-w-nodemcu-esp8266-on-arduino-ide-28184f) to add the NodeMCU configuration.
 
 ### Connect The Two Boards as Shown Below
 - Arduino UNO board communicates serially with the NodMCU board.
 - Connect Arduino UNO to the power source using the 5V-2A adapter
 - Using the 5V pin on the Arduino UNO, connect The NodMCU power pin and Unify the ground of the boards by connecting the ground pins as shown in the image below.   
<a href="https://tamra-iot.com/">
<img src="https://elewah.github.io/Tamra/ThingsDriver/schematic_bb.svg" width="250" alt="Tamra Logo" title="Tamra">
</a>

### 3. Citations:
Kindly cite this paper in your research if it helps your work.

[A. Elewah, W. M. Ibrahim, A. Rafıkl and K. Elgazzar, "ThingsDriver: A Unified Interoperable Driver for IoT Nodes," 2022 International Wireless Communications and Mobile Computing (IWCMC), 2022, pp. 877-882, doi: 10.1109/IWCMC55113.2022.9824989.](https://ieeexplore.ieee.org/document/9824989)
 

### 4. About Us:
Tamra is an incubated startup in the Ebni incubator. In 2018, Tamra won seed funding in the ICT track at the InnoEgypt program. Apart from the commercial side, one of the Co-founder is doing his Ph.D. degree in Ontario tech university at IoT lab. He developed ThingsDriver as a small gear on his thesis topic, [Public Sensing](https://iotresearchlab.ca/public-sensing-as-a-service).


 
### 5. Copyright and Licenses

Code released under the [MIT license](LICENSE).

## 
</br>
<a href="https://tamra-iot.com/">
<img src="https://elewah.github.io/Tamra/logos/logo-horizontal.png" width="250" alt="Tamra Logo" title="Tamra">
</a>
<a href="https://ontariotechu.ca/">
<img src="https://elewah.github.io/Tamra/logos/ontariotechu-log.jpg"  alt="Ontariotechu Logo"  width="250">
</a>
<a href="https://iotresearchlab.ca/">
<img src="https://elewah.github.io/Tamra/logos/IoT-lab.png" width="300" alt="IoT-Lab Logo" title="IoT Lab">
</a>
