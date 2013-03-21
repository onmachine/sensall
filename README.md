#sensall

##Overview
This project is a wireless sensor network that communicates with the Web.  Arduino-based Sensors communicate with an Arduino-based Sensor Router that interfaces with a Web Architecture.

##Sensors
The Sensors monitor and potentially act locally on something.  The most basic example is a door sensor that just reports when a door opens and closes.  The Sensors are Arduino-based with XBee modules for wireless communication. Sensors monitors something local, creates a packet of data, and sends it to the router for further communication to the Web.  Optionally the sensor can also be prompted to respond or perform some function, like close the garage door.

##Sensor Router
The router is the interface between the Web and the sensor network.  It may pass the sensor data to the Web or receive a request from the Web and get the data from the sensors.

##Web Architecture
The architecture for processing and accessing the data is likely to evolve.  The current structure:

The Sensor Router uses an HTTP Get to send data to [PushingBox](http://pushingbox.com/), a very useful and free service specifically for connecting microcontrollers with the Web.   PushingBox then passes that data to a Google Form, which, as a function of Google Forms, writes to a Google Spreadsheet.

The Google Spreadsheet data is formatted and augmented by client-side HTML5 technologies, which can be hosted anywhere, such as GitHub Pages.
