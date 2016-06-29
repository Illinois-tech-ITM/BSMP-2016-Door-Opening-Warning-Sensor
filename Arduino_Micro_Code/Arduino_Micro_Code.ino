#include "Maxbotix/Maxbotix.h"
#include "NeoPixel/Neopixel.h"

#define SONIC_SENSOR_PIN 3  // Pin where the Sonic Rangefinder Controller is
#define SOUNDPIN A5         // Pin where the buzzer signal is
#define SOUNDFREQ 440       // Sound Frequency of Buzzer
#define LEDPIN A0           // Pin where warning NeoPixel ring is
#define NUMPIXELS 16        // Number of pixels in the NeoPixel ring
#define MAXDIST 200         // Maximum distance to be "too far away from door"
#define MOVTOLERANCE 5      // Tolerance of distance sensor (in cm) to be considered stopped in place (empirical)
#define SAMPLE_SIZE 50      // Number of samples to analyze to decide if warning should continue ringing
#define KERNEL_SIZE 10      // Size of the low-pass filter

//#define DEBUG
//#define DEBUG_RANGE
#define DEBUG_FILTER

/********************************************
 *  Functions, Enums and Global Vars declaration   *
 ********************************************/
float lerp(float a, float b, float i);
void solid(uint8_t R, uint8_t G, uint8_t B);
void pushToFilter(float value);
float updateMean();

Maxbotix rangeSensorPW(SONIC_SENSOR_PIN, Maxbotix::PW, Maxbotix::LV, Maxbotix::NONE); // Class for using the sonic rangefinder
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800); // Class for controlling LED ring
boolean warningOn = false; // For controlling the buzzer toggle
byte notApproachingCount = 0, approachingCount = 0;
float lastRange = 999;
float ranges[KERNEL_SIZE];
float treatedRange;

enum ObjectState {
  APPROACHING,
  STOPPED,
  DISTANCING
};

/********************************
 *  Arduino-Specific Functions  *
 ********************************/
void setup() {
  
#if defined(DEBUG) || defined(DEBUG_RANGE) || defined(DEBUG_FILTER)
  Serial.begin(9600);
  while(!Serial);
#endif

  pixels.begin();
  pinMode(SOUNDPIN, OUTPUT);
}

void loop() {
  pushToFilter(rangeSensorPW.getRange()); // Distance (in cm) of closest detected object from Sonic Rangefinder
  float delta = -(treatedRange - updateMean());

#if defined(DEBUG) || defined(DEBUG_FILTER)
  Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  for(int i = 0; i < KERNEL_SIZE-1; i++){
    Serial.print(ranges[i]);Serial.print(" | ");    
  }
  Serial.println(ranges[KERNEL_SIZE-1]);
  Serial.print("Mean: ");Serial.println(treatedRange);
  Serial.print("delta: ");Serial.println(delta);
#endif
  /*
#if defined(DEBUG) || defined(DEBUG_RANGE)
  Serial.println("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
  Serial.print("Range: ");Serial.print(range);Serial.println("\tcm.");
  Serial.print("lastRange: ");Serial.print(lastRange);Serial.println("\tcm.");
  Serial.print("lastRange-range (");Serial.print(lastRange - range);Serial.print(") < ");Serial.print(MOVTOLERANCE);Serial.println(lastRange-range<MOVTOLERANCE?" = TRUE":" = FALSE");
  Serial.print("approachingCount = ");Serial.println(approachingCount);
  Serial.print("notApproachingCount = ");Serial.println(notApproachingCount);
//  delay(500);
#endif

  if (lastRange-range < MOVTOLERANCE) {
    notApproachingCount = notApproachingCount<SAMPLE_SIZE?notApproachingCount+1:SAMPLE_SIZE;
    approachingCount = 0;
  } else {
    if (++approachingCount >= 2){
      notApproachingCount = 0;
    }
  }
  lastRange = range;
  
  if (range > MAXDIST || notApproachingCount >= SAMPLE_SIZE) { // Turn off LED and Buzzer if person is too far
    noTone(SOUNDPIN);
    solid(0,0,0);
    
  } else { // Turn on otherwise
    float period = lerp(0, 100, range/MAXDIST); // LED Blink ms delay
    static unsigned long t_start = 0;
    unsigned long m = millis();
  #ifdef DEBUG
    Serial.print("LED delay: ");Serial.print(period);Serial.println("\tms.\t");
    Serial.print("t_start: ");Serial.print(t_start);Serial.print("\tms.\t");
    Serial.print("m: ");Serial.print(m);Serial.print("\tms.\t");
    Serial.print("m-t_start: ");Serial.print(m-t_start);Serial.println("\tms.\t");
    Serial.print("LED is ");Serial.print(digitalRead(LEDPIN)?"On.":"Off.");
    Serial.print("\tBuzzer is ");Serial.println(warningOn?"On.":"Off.");
    Serial.println();
  #endif
    
    if(m - t_start >= period) { // Toggling LED and buzzer
      t_start = millis();

      if (warningOn) {
        noTone(SOUNDPIN);
        solid(0,0,0);
      } else {
        tone(SOUNDPIN, SOUNDFREQ);
        solid(255,0,0);
      }
      warningOn = !warningOn;
    
    } // End if Toggle
    
  } // End if Alarm System Distance handling
  */
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

void pushToFilter(float value) {
  for(int i = 0; i<KERNEL_SIZE-1; i++){
    ranges[i] = ranges[i+1];
  }
  ranges[KERNEL_SIZE-1] = value;
}

float updateMean() {
  float m = 0;
  for(int i = 0; i<KERNEL_SIZE; i++){
    m+=ranges[i];
  }
  return treatedRange = m/KERNEL_SIZE;
}

