//Program for opening and closing exhaust
//Version 1
//
//2.19.2021
//By: Ken Zimny

//==========================//
//Included Libraries
//==========================//

//EEPROM
#include <EEPROM.h>

//OTA
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//==========================//
//Definitions
//==========================//

//I/O pins
#define openRelay D2
#define closeRelay D1
#define exhaustButton D5


//==========================//
//Vars and Constants
//==========================//

//Debounce int
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
int buttonReading;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 70;    // the debounce time; increase if the output flickers

//exhaust open/close logic
bool exhaustState = 0;

//button pressed logic
bool buttonPressed = 0;

//OAT
const char* ssid = "";
const char* password = "";


void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(5000);
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  EEPROM.begin(512);
  pinMode(openRelay, OUTPUT);
  pinMode(closeRelay, OUTPUT);
  pinMode(exhaustButton, INPUT_PULLUP);

  //  EEPROM
  exhaustState = EEPROM.read(0);
}


void loop() {
  // put your main code here, to run repeatedly:

  ArduinoOTA.handle();

  //Read the exhaust button from the car
  if (digitalRead(exhaustButton) == LOW) {

    //reset the debouce
    if (digitalRead(exhaustButton) != lastButtonState) {
      lastDebounceTime = millis();
    }
    //debouuce the buttons
    if ((millis() - lastDebounceTime) > debounceDelay) {

      //Open or close the car exhaust
      if (exhaustState == 0 && buttonPressed == 0) {
        Serial.println ("Exhaust opening ");
        openExhaust();
        exhaustState = !exhaustState;
        EEPROM.write(0, exhaustState);
        EEPROM.commit();
        Serial.println ("Written to memory");
        buttonPressed = 1;
      }

      else if (exhaustState == 1 && buttonPressed == 0) {
        Serial.println ("Exhaust closing");
        closeExhaust();
        exhaustState = !exhaustState;
        EEPROM.write(0, exhaustState);
        EEPROM.commit();
        Serial.println ("Written to memory");
        buttonPressed = 1;
      }

    }
  }
  else {
    buttonPressed = 0;
  }
}

//Exhaust open/closed functions
void closeExhaust() {
  digitalWrite(closeRelay, HIGH); //active the close relay
  delay(2000);                    //wait 3 seconds to fully close the flaps
  digitalWrite(closeRelay, LOW);  //power off the relay
  delay(100);                     //delay added as pause between next button press
}

void openExhaust() {
  digitalWrite(openRelay, HIGH);  //active the close relay
  delay(2000);                    //wait 3 seconds to fully close the flaps
  digitalWrite(openRelay, LOW);   //power off the relay
  delay(100);                     //delay added as pause between next button press
}
