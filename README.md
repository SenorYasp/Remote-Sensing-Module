# Remote-Sensing-Module
A standalone composite of hardware and software to record environmental data and transmit said data wirelessly   

Components used in this system:\
-->[Arduino Mega 2560](https://store.arduino.cc/usa/arduino-mega-2560-rev3)\
-->[Whitebox Labs Tentacle Shield](https://www.atlas-scientific.com/product_pages/components/tentacle-shield.html)\
-->[Altas Scientific EZO sensors](https://www.atlas-scientific.com/product_pages/circuits/ezo_ph.html)\
-->[Adafruit datalogger](https://learn.adafruit.com/adafruit-data-logger-shield)\

The Arduino code consists of a datalogging library. To accurately represent your hardware setup, change total_circuits, channel_ids, and channel_names in the datalogging header file DataLogLib.h. In tentacle_to_Dweet, update "yourThing" to approriately name your device. 
