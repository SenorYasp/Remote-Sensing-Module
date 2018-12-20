#ifndef DataLog_l
#define DataLog_l

#if (ARDUINO >= 100)
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <Wire.h>
#include <SD.h>
#include <RTClib.h>


class DataLog {
  public:
    DataLog(bool dispMsg = false);
    void begin(long baudRate = 9600);
    String getData();
    String allData = "";


  private:
    //methods
    String getTimeString(DateTime time);
    DateTime getTime();
    File weekCheck();
    File getFile();
    void sleep();


    //variables
    bool _msg;
    int week = 0;
    char sensordata[30];
    byte sensor_bytes_received = 0;
    byte code = 0;
    byte in_char = 0;
    int total_circuits = 2;
    int channel_ids[2] = {97, 102};
    char *channel_names[2] = {"DO", "RTD"};
    DateTime _start;
    long start;
    int channel;

};

#endif
