# Remote-Sensing-Module
A standalone composite of hardware and software to record environmental data and transmit said data wirelessly   

Components used in this system:\
-->Arduino Mega 2560\
-->Whitebox Labs Tentacle Shield\
-->Altas Scientific EZO sensors\
-->Adafruit datalogger\
-->FONA mini GSM 800h\

The Arduino code consists of a datalogging library and a data transmission sketch. To accurately represent your hardware setup, change total_circuits, channel_ids, and channel_names in the datalogging header file DataLogLib.h. In tentacle_to_Dweet, update "yourThing" to approriately name your device. 
