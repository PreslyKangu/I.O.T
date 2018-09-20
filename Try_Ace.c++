#include "Adafruit_FONA.h"
#include <SoftwareSerial.h>

//GSM
#define FONA_RX 2
#define FONA_TX 3
#define FONA_RST 4
const char phoneNum[10]= "0704658304";

#define fireSensor 5
#define ldr A0
int ledPin = 13;
//int ldrPin = A0;
//RGB
#define blue A5
#define green A4
#define red A3
//definition of constants
#define MinLight 100

int pirSensor = 8;
int relayInput = 7;
int LED = 12;

//GSM objects declaration
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;

// Use this for FONA 800 and 808s
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);

uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);


class controller{

  public:
      int sensorValue = digitalRead(pirSensor); //for motion
      int fireSignal=digitalRead(fireSensor); //fire
      int light=analogRead(ldr); //light
    //global variables for the class
    

          //function to detect intruder
            bool detectMotion()
                  {
                    if (sensorValue == 1) {
                      //intruder present
                      Serial.println("Intruder Present!!");
                      return true;
                    }
                    else
                    {
                      //digitalWrite(relayInput, 0);
                      return false;
                    } 
                  }
      
             bool detectFire()
             {
                if (fireSignal==1)
                {
                  Serial.println("Fire Present!");
                  return true;
                }
                else
                {
                  return false;
                }
             }
      
            bool detectNight()
            {
              if(light<MinLight)
              {
                return true;
              }
              else
              {
                return false;
              }
            }
      
          //actuators
          void startBuzzer()
          {
            digitalWrite(relayInput,1);
          }
          void startBuzzer(int Delay)
          {
            digitalWrite(relayInput,1);
            delay(Delay);
            digitalWrite(relayInput,0);
            delay(Delay);
          }
          void startBuzzer(int Delayone, int Delaytwo)
          {
            digitalWrite(relayInput,1);
            delay(Delayone);
            digitalWrite(relayInput,0);
            delay(Delaytwo);
          }
      
           void stopBuzzer()
          {
            digitalWrite(relayInput,0);
          }
      
           void lightBulb()
          {
            digitalWrite(ledPin,1);
          }
          void closeBulb()
          {
            digitalWrite(ledPin,0);
          }
       //colors for different scenarios using RGB
            void redLight()
            {
                analogWrite(red,255);
                analogWrite(blue,0);
                analogWrite(green,0);
            }

             void greenLight()
            {
                analogWrite(red,0);
                analogWrite(blue,0);
                analogWrite(green,200);
            }
             void blueLight()
            {
                analogWrite(red,0);
                analogWrite(blue,255);
                analogWrite(green,0);
            }
             void blueGreenLight()
            {
                analogWrite(red,204);
                analogWrite(blue,204);
                analogWrite(green,255);
            }

            //GSM Methods
            void flushSerial() 
          {
            while (Serial.available())
              Serial.read();
          }
          
          void makeCall(char number[10])
          {
             // call a phone!
                  
                  flushSerial();
                  Serial.print(F("Calling ")); Serial.println(number);
                  if (!fona.callPhone(number)) {
                    Serial.println(F("Failed"));
                  } else {
                    Serial.println(F("Calling..."));
                  }
          }
          
          void sendSMS(char message[141], char phone[10])
          {
            // send an SMS!
                  flushSerial();
                  Serial.println(message);
                  if (!fona.sendSMS(phone, message)) {
                    Serial.println(F("Failed"));
                  } else {
                    Serial.println(F("Sent!"));
                  }
          }
            
};


void setup() {
  //serial monitor
  Serial.begin(115200);
  /*
  while (!Serial);
  Serial.begin(115200);
  Serial.println(F("Initializing GSM"));
  Serial.println(F("Initializing....(May take up to 3 seconds)"));

  //checking availability
  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println(F("GSM developed a network issue \npossibly lack of SIM-Card "));
    while (1);
  }*/
    //inputs
    pinMode(fireSensor,INPUT);
    pinMode(pirSensor, INPUT);
    pinMode(ldr, INPUT);

    //Outputs
    pinMode(ledPin, OUTPUT);
    pinMode(relayInput, OUTPUT);
    //RGB
    pinMode(red, OUTPUT);
    pinMode(green, OUTPUT);
    pinMode(blue, OUTPUT);

   //serial monitor
  //Serial.begin(9600); 
}


void loop() {     
      
      controller control;
     // control.redLight();

    
      if(control.detectFire() && control.detectMotion())
      {
        //if both a motion and fire, we light Red, and start buzzer
        Serial.println("System detected unusual motion and fire!!");
        control.redLight();
        control.startBuzzer();
       // control.sendSMS("The system detected unusual motion and Fire",phoneNum);
       // delay(20000);
       // control.makeCall(phoneNum);
      }
      else if(control.detectFire())
      { 
         Serial.println("fire detected");
        control.startBuzzer(500);
        //if we detect fire, we light a blue light & start buzzer
         control.blueLight();
         //control.sendSMS("System detected Fire.",phoneNum);
        // delay(20000);
        
      }
      else if (control.detectMotion())
      {
        control.startBuzzer(1000,500);
        Serial.println("intruder present");
        
          //if we detect motion, we light maroon and start a buzzer
        control.blueGreenLight();
       // control.sendSMS("System unusual motion in your house",phoneNum);
        // delay(20000);
      }
      else
      {
        //if we have nothing unusual, we use a green light
        control.greenLight();
        Serial.println("Everything is usual!");
      }

      //light
      if(control.detectNight())
      {
        //if it is night time, light bulb
        control.lightBulb();
      }
      else
      {
        //switch off the bulb otherwise
        control.closeBulb();
      }
      
 }
