//to have the newPingLibrary working with the tone function you must comment teh following on newPingg.cpp lib code:
/*
#if defined (__AVR_ATmega32U4__) // Use Timer4 for ATmega32U4 (Teensy/Leonardo).
ISR(TIMER4_OVF_vect) {
#else
ISR(TIMER2_COMPA_vect) {
#endif
  if(intFunc) intFunc(); // If wrapped function is set, call it.
}
*/

// Bounce library and new ping library decalration
#include <Bounce2.h>
#include <NewPing.h>
#define MAX_DISTANCE 400   // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
int switch2 = 11;          // Push button to activate or desactivate alarm sound
int echo = 2;              // Echo Pin of the ultrasonic sensor
int trigger = 3;           // Trigger pin ofthe ultrasonic sensor
int buzzer = 13;           // Sound alarm buzzer
int pir = 10;              // PIR sensor Pin
int distance = 0;          // Default distance measured by ultrasonic sensor
boolean alarm = LOW;       // Alarm state LOW Sound trigger, HIGH no sound
// the time when the sensor outputs a low impulse
long unsigned int lowIn;
// the amount of milliseconds the sensor has to be low
// before we assume all motion has stopped
long unsigned int pause = 5000;
boolean lockLow = true;
boolean takeLowTime;
int calibrationTime = 30;
// sinusoid alarm sound variable
float sinVal;
int toneVal;
// Instance creation for the bounce library
Bounce switch2Bounce = Bounce();
// Instance creation for the New ping Library
NewPing sonar(trigger, echo, MAX_DISTANCE);


void setup() {
  
  Serial.begin(9600);
  pinMode(pir, INPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(switch2,INPUT);
  // Attaching the switch2 to the bounce object with a 100ms bounce intervall
  switch2Bounce.attach(switch2); 
  switch2Bounce.interval(100);
  // activation of the pull up resistor for the PIR pin
  digitalWrite(pir, HIGH);
  // give the sensor some time to calibrate
  Serial.print("calibrating sensor ");
  for (int i = 0; i < calibrationTime; i++) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);
}

////////////////////////////
//LOOP
void loop()   {
  readPIR();      // Call the function to Read the PIR  sensor
  alarmState();  // Call the function to check the state of the switch2
}
//////////////////
//PIR Funcrion
void readPIR() {
  if (digitalRead(pir) == HIGH) {
    
    soundAlarm();      //Call the function to sound or not the alarm when motion is detected
    pingDist();        // Call the function to measure distance from th ultrasonic sensor if motion is detected
    if (lockLow) {
      //makes sure we wait for a transition to LOW before any further output is made:
      lockLow = false;
      Serial.println("---");
      Serial.print("motion detected at ");
      Serial.print(millis() / 1000);
      Serial.println(" sec");
      delay(50);
    }
    takeLowTime = true;
  }

  if (digitalRead(pir) == LOW) {
    if (takeLowTime) {
      lowIn = millis();          //save the time of the transition from high to LOW
      takeLowTime = false;       //make sure this is only done at the start of a LOW phase
    }
    //if the sensor is low for more than the given pause,
    //we assume that no more motion is going to happen
    if (!lockLow && millis() - lowIn > pause) {
      //makes sure this block of code is only executed again after
      //a new motion sequence has been detected
      lockLow = true;
      Serial.print("motion ended at ");      //output
      Serial.print((millis() - pause) / 1000);
      Serial.println(" sec");
      delay(50);
    }
  }
}
//////////////
// Ultrasonic sensor distance reading
void pingDist () {
  delay(50);                      // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.
  distance = sonar.convert_cm(sonar.ping_median(10));
  Serial.print("Distance");
  Serial.println(distance); // Convert ping time to distance in cm and print result (0 = outside set distance range)
}
////////////////////
// Alarm Sound a sinusiod is generated and outputed on the buzzer pin
void soundAlarm() {
  
  if (alarm==LOW){
  for (int x = 0; x < 180; x++) {
    // convert degrees to radians then obtain value
    sinVal = (sin(x * (3.1412 / 180)));
    // generate a frequency from the sin value
    toneVal = 2000 + (int(sinVal * 1000));
    tone(buzzer, toneVal);
    delay(1);
 }
 
 noTone(buzzer);
  }
}
/////////////////
//Check the status of the switch and detect the falling edge.
void alarmState() {
 if (switch2Bounce.update()) {
    if (switch2Bounce.fell()) {
      alarm = !alarm;
      Serial.print("Alarm=");
      Serial.println(alarm);
    }  
} 
}

