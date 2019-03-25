/*****************************************

Hydro-Electric-Generator Application
      By Declan O'Connor

Blynk library is licensed under MIT license
******************************************/


//Import the following
#include <BlynkSimpleStream.h>
#include <SoftwareSerial.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

//Configuration for information over serial
#define BLYNK_PRINT SwSerial
SoftwareSerial SwSerial(10, 11); // RX, TX

//Authentication Key for Blynk App
char auth[] = "7275d08c0cb64bbf8b384d9f2c9f215c"; 

//Intialise the following
int LED = 6;
int LED2 = 9;

//Initalise the following
WidgetTerminal terminal(V2);
WidgetRTC rtc;
BlynkTimer timer;

//Gauge Virtual Pin
BLYNK_WRITE(V1)
          {
           /*  int pinValue = param.asInt();  // assigning incoming value from pin V1 to a variable  // process received value*/
          int pinValue = analogRead (A0); 
          } 

//Terminal Virtual Pin
BLYNK_WRITE(V2)
{
  //Declare variables
  int pinValue = analogRead (A0);
  int voltage = pinValue / 204;

  //Display commands when prompted
  if (String("help") == param.asStr()) {
      terminal.print(F("--------------------------------------------"));
      terminal.print(F("COMMANDS------------------------------------"));
      terminal.print(F("--------------------------------------------"));
      terminal.print(F("help:    Displays commands                    "));
      terminal.print(F("voltage: Displays voltage output                    "));
      terminal.print(F("clear:   Clears terminal window                     "));
  }

  //Clear the terminal when prompted
  if (String("clear") == param.asStr()) {
     terminal.clear();
  }

  //Display the Voltage output when prompted
  if (String("voltage") == param.asStr()) {
    terminal.print(F("GENERATOR IS CURRENTLY RUNNING AT:"));
    terminal.print(voltage);
    terminal.print(F("V"));
    terminal.print(F("                                               "));
  } 

  //Everything else besides help returns the following message
  if (String("help") != param.asStr()) 
  {
    terminal.print("You said: '");
    terminal.write(param.getBuffer(), param.getLength());
    terminal.print("' Type 'help' for anything else.");
    terminal.println();
  }
  
  // Ensure everything is sent
  terminal.flush();
}

// Digital clock display of the time
void clockDisplay()
{
  setTime(1, 1, 1, 25, 3, 2019);
  // You can call hour(), minute(), ... at any time
  // Please see Time library examples for details
  String currentTime = String(hour()) + ":" + minute() + ":" + second();
  String currentDate = String(day()) + " " + month() + " " + year();
  SwSerial.print("Current time: ");
  SwSerial.print(currentTime);
  SwSerial.print(" ");
  SwSerial.print(currentDate);
  SwSerial.println();

  // Send time to the App
  Blynk.virtualWrite(V3, currentTime);
  // Send date to the App
  Blynk.virtualWrite(V4, currentDate);
}

void setup()
    {
      //Intialise pinmode type for LEDs
      pinMode(LED, OUTPUT);
      pinMode(LED2, OUTPUT);
      // Blynk config to work through serial
      SwSerial.begin(9600);
      Serial.begin(9600);
      Blynk.begin(Serial, auth);
      // Begin synchronizing time
      rtc.begin();
      // Display digital clock every 1 second
      timer.setInterval(1000L, clockDisplay);
      //Configure terminal to be clear and show welcome  
      terminal.clear();
      terminal.println(F("Welcome to Declan's Hydro-Electric-Monitor"));
      terminal.flush(); 
    }

void loop()
    {
     //Run Blynk elements
     Blynk.run();
     //Start timer
     timer.run();
     //If Input exceeds value turn on warning light and off neutral light and display notification
     int pinValue = analogRead (A0);
     if (pinValue >= 700) //light the LED;
            {
              digitalWrite(LED, HIGH);
              digitalWrite(LED2, LOW);
              delay(1000);
              terminal.print(F("********"));
              terminal.print(F("DANGER-WATER-LEVELS-RISING"));
                  Blynk.notify("DANGER-WATER-LEVELS-RISING");
              terminal.print(F("********")); 
              terminal.flush(); 
            }
              else
            {
              //turn off warning light and turn on neutral light and resume
              digitalWrite(LED, LOW);
              digitalWrite(LED2, HIGH);
              Blynk.run();
            }
     }
