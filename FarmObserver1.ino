/*
    FarmChecker1
 */

#include <WiFi.h>
#include "private_config.h"
#include <Wire.h>
#include <stdio.h>
#include "lib/SparkFunBME280.h"

BME280 mySensor;

WiFiServer server(80);

// Write Log
//  Debugging : Serial Log
//  Production: log text?
void write_log(const char* message, ...)
{
  char log[1024];
  sprintf(log, message, ...);
  Serial.println(log);
}

// setup function
void setup()
{
    Serial.begin(SERIAL_SPEED);
    pinMode(5, OUTPUT);      // set the LED pin mode

    delay(10);

    // Start by connecting to a WiFi network
    if(setup_wifi() == false){
      write_log("failed connecting wifi network.");
      return;
    }
    
    // Start Sensor Settings
    if(setup_sensor_bme280() == false){
      write_log("failed setup sensor bm280");
      return;
    }

    server.begin();
}

bool setup_wifi()
{
  write_log("Wifi Connecting to");
  write_log(MYSSID);

  WiFi.begin(MYSSID, SSIDPASS);

  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      write_log(".");
      count++;
      if(count >= 10)
        return false;
  }

  write_log("WiFi connected.");
  write_log("IP address: ");
  write_log(WiFi.localIP());

  return true;
}

// ==========================================
// ==== BME280 Sensor Function ==============
// ==========================================
bool setup_sensor_bme280()
{
  if (mySensor.beginI2C() == false) //Begin communication over I2C
  {
    write_log("The sensor did not respond. Please check wiring.");
    return false;
  } 

  return true;
}

// 
bool read_sensor_bme280()
{
  write_log("Humidity: %f", mySensor.readFloatHumidity());

  write_log(" Pressure: %f", mySensor.readFloatPressure());

  write_log(" Alt: %f", mySensor.readFloatAltitudeFeet());

  write_log(" Temp: %f", mySensor.readTempF());
}

int value = 0;

void SaveData(){
  
}

void loop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    write_log("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 5 on.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 5 off.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(5, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(5, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    write_log("Client Disconnected.");
  }
}