#include "Maxbotix/Maxbotix.h"

#define SONIC_SENSOR_PIN 8  // Pin where the Sonic Rangefinder Controller is
#define SOUNDPIN A4         // Pin where the buzzer signal is
#define LEDPIN A3           // Pin where warning LED is
#define MAXDIST 200         // Maximum distance to be "too far away from door"

//#define DEBUG

/********************************************
 *  Functions and Global Vars declaration   *
 ********************************************/
float lerp(float a, float b, float i);

Maxbotix rangeSensorPW(SONIC_SENSOR_PIN, Maxbotix::PW, Maxbotix::LV, Maxbotix::BEST);
boolean buzzing = false; // For controlling the buzzer toggle


/********************************
 *  Arduino-Specific Functions  *
 ********************************/
void setup() {
#ifdef DEBUG
  Serial.begin(9600);
  while(!Serial);
#endif

  pinMode(SOUNDPIN, OUTPUT);
  pinMode(LEDPIN, OUTPUT);
}

void loop() {
  float range = rangeSensorPW.getRange(); // Distance (in cm) of closest detected object from Sonic Rangefinder
#ifdef DEBUG
  Serial.print("Range: ");Serial.print(range);Serial.println("\tcm.");
#endif

  if (range > MAXDIST) { // Turn off LED and Buzzer if person is too far
    noTone(SOUNDPIN);
    digitalWrite(LEDPIN, LOW);
    
  } else { // Turn on otherwise
    float freq = lerp(1760, 55, range/MAXDIST); // Sound Frequency of Buzzer
    float freq2 = lerp(1, 1000, range/MAXDIST); // LED Blink Frequency
    static unsigned long t_start = 0;
    unsigned long m = millis();
  #ifdef DEBUG
    Serial.print("freq: ");Serial.print(freq);Serial.print("\thz.\t");
    Serial.print("freq2: ");Serial.print(freq2);Serial.println("\thz.\t");
    Serial.print("t_start: ");Serial.print(t_start);Serial.print("\tms.\t");
    Serial.print("m: ");Serial.print(m);Serial.print("\tms.\t");
    Serial.print("m-t_start: ");Serial.print(m-t_start);Serial.println("\tms.\t");
    Serial.print("LED is ");Serial.print(digitalRead(LEDPIN)?"On.":"Off.");
    Serial.print("\tBuzzer is ");Serial.println(buzzing?"On.":"Off.");
  #endif
    
    if(m - t_start >= freq2) { // Toggling LED and buzzer
      digitalWrite(LEDPIN, !digitalRead(LEDPIN));
      t_start = millis();

      buzzing = !buzzing;
      if (buzzing) {
        noTone(SOUNDPIN);
      } else {
        tone(SOUNDPIN, freq);
      }
    
    } // End if Toggle
    
  } // End if Alarm System Distance handling
  
} // End Loop


/*******************
 *  Our Functions  *
 *******************/
float lerp(float a, float b, float i) {
  return (1-i)*a + i*b;
}
