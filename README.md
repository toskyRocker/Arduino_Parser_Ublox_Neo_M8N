# Arduino_Parser_Ublox_Neo_M8N
Simple C++ library to parse U-Blox Neo M8N strings. 
Software tested on Atmega32u4 (Arduino Yun/Micro/Leonardo)

----- Simple Arduino Parser for U-Blox Neo M8N (GPS/GLONASS receiver) -----

Some of the obtained info:
 - altitude, latitude, altitude
 - fix type
 - satellites in view 
 - setellites in use
 - pdop, hdop, vdop, dop Age
 - date, time
 - speed, course, knots
 - speed_age, course_age, knots_age
 - satellite: prn, elevation, azimuth, snr
 

No virtual Serial (SoftwareSerial) is required since I use Serial1 on the Atmega32u4
 
Parsed strings:
 - GNGGA, GNGSA, GPGSV, GNRMC, GNVTG
 
 
----- The project includes also the txt file describing the right M8N configuration for multirotors -----

You can flash the txt configuration file on the M8N using U-Center
