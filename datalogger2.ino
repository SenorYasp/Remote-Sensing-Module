

/*
 * HWS data collection for blue green algae research 
 * Research under Professor Ileana Dumitriu 
 * 
 * Code manipulation by Jasper White
 * 
 * Date Modified: 8/27/19
 * 
 */


#include <Wire.h>                     // enable I2C.
#include <SD.h>
#include <RTClib.h>
#include <LowPower.h>


File logfile;
RTC_PCF8523 rtc;
long sampleTime;
long sleep;

char sensordata[30];                  // A 30 byte character array to hold incoming data from the sensors
byte sensor_bytes_received = 0;       // We need to know how many characters bytes have been received

byte code = 0;                        // used to hold the I2C response code.
byte in_char = 0;                     // used as a 1 byte buffer to store in bound bytes from the I2C Circuit.

//int i = 0;
String data = "";
int channel;

/* Change the next 3 lines to match your setup.
 * Run tentacle_info.ino to scan for sensors 
 * 
 * --> TOTAL_CIRCUITS = number of sensors
 * --> channel_ids[] = i2c address of sensors 
 * --> channel_names must match order of channel_ids
 * --> runtime = inverval of time between readings (sec)
 */

#define TOTAL_CIRCUITS 4                                    // <-- CHANGE THIS 
int channel_ids[] = {65,97, 64, 102};                      // <-- CHANGE THIS.
char *channel_names[] = {"DO_1","DO_2","RTD_1","RTD_2"}; // <-- CHANGE THIS.
int sampleint = 30;    //minutes
int offtime = sampleint*60/8 - 14;
//int delaytime = sampletime*54 %(offtime*8 + TOTAL_CIRCUITS);


void setup() {                      // startup function
  Serial.begin(9600);               // Set the hardware serial port.
  Wire.begin();                     // enable I2C port.
  rtc.begin();                      // initialize RTC


  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);
  
  // see if the card is present and can be initialized:
  while (!SD.begin(10)) {
    Serial.println("Card failed, or not present");
    delay(4000);
  }
  Serial.println("card initialized.");


  // create a new file
  char filename[] = "FROGYS00.CSV";
  for (uint8_t i = 0; i < 100; i++) {
    filename[6] = i/10 + '0';
    filename[7] = i%10 + '0';
    if (! SD.exists(filename)) {
      // only open a new file if it doesn't exist
      logfile = SD.open(filename, FILE_WRITE);
      String header = "date,time,";
      for (int i = 0; i < TOTAL_CIRCUITS; i++){
        header += String(channel_names[i]) + ",";
      }
      logfile.println(header);
      break;  // leave the loop!
    }
  }
  
  Serial.print("Logging to: ");
  Serial.println(filename);


  DateTime starttime = rtc.now();
  while ( ! ((starttime.minute() == 59 && starttime.second() == 60-TOTAL_CIRCUITS) || (starttime.minute() == 29 && starttime.second() == 60-TOTAL_CIRCUITS))){
    Serial.println("Waiting to start data collection on the hour or half-hour");
    delay(1000);
    starttime = rtc.now();
    Serial.println(getTimeString(starttime));
     
  }
}


void loop() {

  data = "";

  for (channel = 0; channel < TOTAL_CIRCUITS; channel++) {       // loop through all the sensors
  
    Wire.beginTransmission(channel_ids[channel]);     // call the circuit by its ID number.
    Wire.write('r');                          // request a reading by sending 'r'
    Wire.endTransmission();                         // end the I2C data transmission.
    
    delay(1000);  // AS circuits need a 1 second before the reading is ready

    sensor_bytes_received = 0;                        // reset data counter
    memset(sensordata, 0, sizeof(sensordata));        // clear sensordata array;

    Wire.requestFrom(channel_ids[channel], 48, 1);    // call the circuit and request 48 bytes (this is more then we need).
    code = Wire.read();

    while (Wire.available()) {          // are there bytes to receive?
      in_char = Wire.read();            // receive a byte.

      if (in_char == 0) {               // null character indicates end of command
        Wire.endTransmission();         // end the I2C data transmission.
        break;                          // exit the while loop, we're done here
      }
      else {
        sensordata[sensor_bytes_received] = in_char;      // append this byte to the sensor data array.
        sensor_bytes_received++;
      }
    }

    sensorSleep();

    DateTime sample = rtc.now();
    String timeStamp = getTimeString(sample);
    data += String(sensordata) + ",";
    

    switch (code) {                       // switch case based on what the response code is.
      case 1:                             // decimal 1 means the command was successful.
        if (channel == TOTAL_CIRCUITS - 1){
          sampleTime = sample.unixtime();
          Serial.println(sampleTime);
          Serial.print("Time:  " + timeStamp);
          Serial.println(data + "     ");
          logfile.print(timeStamp);
          logfile.println(data);
          logfile.flush();
        }
        break;                              // exits the switch case.
      
      case 2:                             // decimal 2 means the command has failed.
        Serial.println("command failed");   // print the error
        break;                              // exits the switch case.

      case 254:                           // decimal 254  means the command has not yet been finished calculating.
        Serial.println("circuit not ready"); // print the error
        break;                              // exits the switch case.

      case 255:                           // decimal 255 means there is no further data to send.
        Serial.println("no data");          // print the error
        break;                              // exits the switch case.
    }

  } // for loop 

  while ( rtc.now().unixtime() <= sampleTime + sampleint*60-TOTAL_CIRCUITS-9){
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
//    sleep = rtc.now().unixtime();
//    delay(8000);
    Serial.println(rtc.now().unixtime());
  }
  delay((sampleTime + sampleint*60-rtc.now().unixtime()-TOTAL_CIRCUITS)*1000);
//
////  keep the board in low power mode until ample time has passed
//  for(int i = 0; i < offtime; i ++){
//    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
//  }
  
//  delay(4000);    // delay between every reading
  
  
} //void loop

String getTimeString(DateTime time){
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
void sensorSleep() {
  Wire.beginTransmission(channel_ids[channel]);     //puts sensor into sleep mode
  Wire.write('sleep');
  Wire.endTransmission();
}
