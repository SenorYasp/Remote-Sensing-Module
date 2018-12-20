/*
   This datalogging library is meant to be used in conjuncture with
   WhiteBox Labs' Tentacle Shield with EZO sensors connected. The
   core of the code is taken from WhiteBox, and SD library examples.

   Some variables need to be altered based on your hardware setup:
      --> total_circuits = number of EZO sensors
      --> channel_ids = I2C addresses for sensors
      --> channel_names = names for sensors (should match order with channel_ids)
      ~~~~~~~ These variables can be changed in the <DataLogLib.h> file ~~~~~~~~~

   The main function of this library is getData() which returns the data
   and time in a comma delineated String format.

   Library created: 11/16/18
   creator: Jasper White
*/



#include "DataLogLib.h"
long start;

DataLog::DataLog(bool dispMsg) {
  _msg = dispMsg;
  week = 0;
  channel = 0;

}

void DataLog::begin(long baudRate) {
  Serial.begin(baudRate);
  if (_msg) {
    Serial.println(" -->  DataLog constructor instantiated successfully");
    Serial.print(" -->  baudRate is:  ");
    Serial.println(baudRate);
  }

  //begin I2C
  Wire.begin();
  Serial.println(" -->  I2C connected");

  //begin RTC
  DateTime _start = getTime();
  start = _start.unixtime();
  Serial.println(" -->  Start time is: " + getTimeString(_start));

  //initialize SD card
  Serial.print(" -->  Initializing SD card . . . ");
  pinMode(SS, OUTPUT);
  while (!SD.begin(53, 11, 12, 13)) {
    Serial.println("card failed, or not present");
    delay(4 * 1000);
  }
  Serial.println("card initialized");
}

DateTime DataLog::getTime() {
  RTC_DS1307 rtc;
  rtc.begin();
  DateTime now = rtc.now();
  return now;
}

String DataLog::getTimeString(DateTime time) {
  String Time = "";
  Time += String(time.month()) + "/";
  Time += String(time.day()) + "/";
  Time += String(time.year()) + ",";
  Time += String(time.hour()) + ":";
  if (time.minute() < 10) Time += "0";
  Time += String(time.minute()) + ":";
  if (time.second() < 10) Time += "0";
  Time += String(time.second());
  Time += ",";

  return Time;
}

String DataLog::getData() {

  allData = "";
  for (channel = 0; channel < total_circuits; channel++) { //(int channel = 0; channel < total_circuits); channel++) {
    String dataString = "";

    Wire.beginTransmission(channel_ids[channel]);
    Wire.write("r");
    Wire.endTransmission();

    String timeString = getTimeString(getTime());

    delay(1000);
    sensor_bytes_received = 0;
    memset(sensordata, 0, sizeof(sensordata));

    Wire.requestFrom(channel_ids[channel], 48, 1);
    code = Wire.read();

    while (Wire.available()) {          // are there bytes to receive?
      in_char = Wire.read();            // receive a byte.

      if (in_char == 0) {               // null character indicates end of command
        sleep();
        //        Wire.endTransmission();         // end the I2C data transmission.
        break;                          // exit the while loop, we're done here
      }
      else {
        sensordata[sensor_bytes_received] = in_char;      // append this byte to the sensor data array.
        dataString += String(sensordata[sensor_bytes_received]);
        sensor_bytes_received++;
      }
    }

    allData += String(channel_names[channel]);
    allData += ",";

    File dataFile = weekCheck();

    switch (code) {
      case 1:
        //        Serial.println("data aquired");
        allData += dataString + "," + timeString;

        if (channel == total_circuits - 1) {
          dataFile.println(allData);
          dataFile.flush();
          return allData;
        }
        break;

      case 2:
        //        Serial.println("command failed");   // print the error

        allData += "command_failed," + timeString;

        if  (channel == total_circuits - 1) {
          //dataFile.println(allData);
          //dataFile.flush();
          return allData;
        }
        break;

      case 254:                              // decimal 254  means the command has not yet been finished calculating.
        //        Serial.println("circuit not ready"); // print the error

        allData += "circuit_not_ready," + timeString;

        if  (channel == total_circuits - 1) {
          //          dataFile.println(allData);
          //          dataFile.flush();
          return allData;
        }
        break;

      case 255:                              // decimal 255 means there is no further data to send.
        //        Serial.println("no data");          // print the error

        allData += "no_data," + timeString;

        if  (channel == total_circuits - 1) {
          //          dataFile.println(allData);
          //          dataFile.flush();
          return allData;
        }
        break;
    }   //switch case
  }   //for loop
}   //getData()

void DataLog::sleep() {
  Wire.beginTransmission(channel_ids[channel]);     //puts sensor into sleep mode
  Wire.write('sleep');
  Wire.endTransmission();
}

File DataLog::weekCheck() {
  long nowTime = getTime().unixtime();

  if (nowTime >= start + (week + 1) * 240) { //604800) {  //new file every week
    week += 1;
    Serial.print("Week is incrementing ... week = ");
    Serial.println(week);
  }
  return getFile();
}

File DataLog::getFile() {
  String weekString = String(week);
  String filename = "datalog" + weekString + ".csv";
  char charBuff [filename.length() + 1];
  filename.toCharArray(charBuff, filename.length() + 1);

  // Open up the file we're going to log to!
  File newFile = SD.open(charBuff, FILE_WRITE);
  //  Serial.println(filename + " ready to write to");

  if (! newFile) {
    Serial.println("error opening datalog.csv");
    // Wait forever since we cant write data
    while (1) ;
  }
  return newFile;
}
