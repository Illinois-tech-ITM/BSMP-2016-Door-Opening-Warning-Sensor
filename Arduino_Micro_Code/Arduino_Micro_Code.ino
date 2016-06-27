#include "Maxbotix/Maxbotix.h"
#include "NeoPixel/Neopixel.h"

#define SONIC_SENSOR_PIN 3  // Pin where the Sonic Rangefinder Controller is
#define SOUNDPIN A5         // Pin where the buzzer signal is
#define LEDPIN A0           // Pin where warning NeoPixel ring is
#define NUMPIXELS 16        // Number of pixels in the NeoPixel ring
#define MAXDIST 200         // Maximum distance to be "too far away from door"

//#define DEBUG

/********************************************
 *  Functions and Global Vars declaration   *
 ********************************************/
float lerp(float a, float b, float i);
void solid(uint8_t R, uint8_t G, uint8_t B);

Maxbotix rangeSensorPW(SONIC_SENSOR_PIN, Maxbotix::PW, Maxbotix::LV, Maxbotix::NONE);
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);
boolean warningOn = false; // For controlling the buzzer toggle


/********************************
 *  Arduino-Specific Functions  *
 ********************************/
void setup() {
#ifdef DEBUG
  Serial.begin(9600);
  while(!Serial);
#endif
  pixels.begin();
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
    solid(0,0,0);
    
  } else { // Turn on otherwise
    float freq = 440;//lerp(1760, 55, range/MAXDIST); // Sound Frequency of Buzzer
    float freq2 = lerp(0, 100, range/MAXDIST); // LED Blink ms delay
    static unsigned long t_start = 0;
    unsigned long m = millis();
  #ifdef DEBUG
    Serial.print("freq: ");Serial.print(freq);Serial.print("\thz.\t");
    Serial.print("LED delay: ");Serial.print(freq2);Serial.println("\tms.\t");
    Serial.print("t_start: ");Serial.print(t_start);Serial.print("\tms.\t");
    Serial.print("m: ");Serial.print(m);Serial.print("\tms.\t");
    Serial.print("m-t_start: ");Serial.print(m-t_start);Serial.println("\tms.\t");
    Serial.print("LED is ");Serial.print(digitalRead(LEDPIN)?"On.":"Off.");
    Serial.print("\tBuzzer is ");Serial.println(warningOn?"On.":"Off.");
    Serial.println();
  #endif
    
    if(m - t_start >= freq2) { // Toggling LED and buzzer
      t_start = millis();

      if (warningOn) {
        noTone(SOUNDPIN);
        solid(0,0,0);
      } else {
        tone(SOUNDPIN, freq);
        solid(255,0,0);
      }
      warningOn = !warningOn;
    
    } // End if Toggle
    
  } // End if Alarm System Distance handling
  
} // End Loop


/*******************
 *  Our Functions  *
 *******************/
float lerp(float a, float b, float i) {
  return (1-i)*a + i*b;
}

void solid(uint8_t R, uint8_t G, uint8_t B) {
  for (int i = 0; i<NUMPIXELS; i++) {
    pixels.setPixelColor(i, pixels.Color(R,G,B));
    pixels.show();
  }
}

