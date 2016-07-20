#include "Maxbotix/Maxbotix.h"   // Library of Sonic Rangefinder
#include "NeoPixel/Neopixel.h"   // Library of LED Ring

#define SONIC_SENSOR_PIN 3  // Pin where the Sonic Rangefinder Controller is
#define SOUNDPIN A1         // Pin where the buzzer signal will be sent
#define SOUNDFREQ 440       // Sound Frequency of Buzzer
#define LEDPIN A0           // Pin where warning NeoPixel ring is
#define NUMPIXELS 48        // Total number of pixels in all NeoPixel rings
#define MAXDIST 200         // Maximum distance (in cm) to be "too far away from door"
#define SAMPLE_SIZE 30      // Number of samples to analyze to decide if warning should continue ringing
                            // (affects how long someone needs to be stopped or distancing from door for the warning to stop)
#define KERNEL_SIZE 10      // Size of the low-pass filter
                            // (bigger kernel yields less noise but more response latency)


#define DEBUG             // Activate all debug messages
//#define DEBUG_APPROACH    // Activate debug messages of the approaching handler
//#define DEBUG_FILTER      // Activate debug messages of the low pass filter
//#define DEBUG_WARNING     // Activate debug messages of the LED and buzzer controller


/***************************************************
 *  Functions, Enums and Global Vars declaration   *
 ***************************************************/
float lerp(float A, float B, float i);        // Linear Interpolation from A to B controlled by i [0-1]
void toggleWarning();                         // Toggles the warning state (turn LEDs and buzzer on and off)
void solid(uint8_t R, uint8_t G, uint8_t B);  // Puts all LEDs of the ring with the same color (0,0,0 to turn off)
void pushToFilter(float value);               // Add the next read value of distance in to the low pass filter kernel
float updateMean();                           // Updates the global var treatedRange with the new filtered value (and returns its value)

enum ObjectState {    // Enum of possible movements of a detected person
    APPROACHING,
    STOPPED,
    DISTANCING
};

Maxbotix rangeSensorPW(SONIC_SENSOR_PIN, Maxbotix::PW, Maxbotix::LV, Maxbotix::NONE);     // For using the sonic rangefinder
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);    // For controlling LED ring
float ranges[KERNEL_SIZE];      // Kernel for low pass filter (array with last KERNEL_SIZE measures)
float treatedRange;             // Range after filtering
ObjectState state = STOPPED;    // Actual movement of the person



/********************************
 *  Arduino-Specific Functions  *
 ********************************/
void setup() {
  #if defined(DEBUG) || defined(DEBUG_APPROACH) || defined(DEBUG_FILTER) || defined(DEBUG_WARNING)
  {
    Serial.begin(9600);
    while(!Serial);
  }
  #endif // DEBUG || DEBUG_APPROACH || DEBUG_FILTER || DEBUG_WARNING
    pixels.begin();
    pinMode(SOUNDPIN, OUTPUT);
} // setup

void loop() {
    static byte notApproachingCount = 0;   // For controlling when to stop the warning even within danger range
    static byte approachingCount = 0;      // For controlling when to re-start the warning
  
    pushToFilter(rangeSensorPW.getRange());         // Distance (in cm) of closest detected object from Sonic Rangefinder
    float delta = -(treatedRange - updateMean());   // Change in the distance from last measurement

    // A delta of more than 26 cm every 25ms (approx. 10 m/s) is probably an error on the detected distance
    if (delta > -26 && delta < 26) {    // so we just ignore that case
        if (delta < -1.5) {             // Someone is approaching if its distance is decreasing by more than 1.5 cm
            state = APPROACHING;
        } else if (delta > 1.5) {       // Someone is distancing if its distance is increasing by more than 1.5 cm
            state = DISTANCING;
        } else if (delta > -0.5 && delta < 0.5) { // Someone stopped moving if its distance is changing by less than 0.5
            state = STOPPED;
        } // if-2x_else_if delta
    } // if |delta| > 26
  #if defined(DEBUG) || defined(DEBUG_FILTER)
  {
    for(int i = 0; i < KERNEL_SIZE-1; i++) {
      Serial.print(ranges[i]);Serial.print(" | ");    
    }
    Serial.println(ranges[KERNEL_SIZE-1]);
    Serial.print("Mean: ");Serial.println(treatedRange);
    Serial.print("delta: ");Serial.println(delta);
  }
  #endif // DEBUG || DEBUG_FILTER
  #if defined(DEBUG) || defined(DEBUG_FILTER) || defined(DEBUG_APPROACH)
  {
    Serial.print("State: ");Serial.println(APPROACHING==state? "Approaching" : STOPPED==state? "Stopped" : "Distancing");
  }
  #endif // DEBUG || DEBUG_FILTER || DEBUG_APPROACH
  #if defined(DEBUG) || defined(DEBUG_APPROACH)
  {
    Serial.print("approachingCount = ");Serial.println(approachingCount);
    Serial.print("notApproachingCount = ");Serial.println(notApproachingCount);
  }
  #endif // DEBUG || DEBUG_APPROACH
    if (APPROACHING == state) {         // If person is approaching
        // Increase counter of consecutive times the person is approaching
        approachingCount = approachingCount<2?approachingCount+1:2;
        if (approachingCount >= 2) {    // If the counter is >= 2, it's not due to noise
            notApproachingCount = 0;    // Reset counter of consecutive times the person is stopped or distancing
        } 
    } else {      // If person is not approaching (i.e. it's stopped or distancing)
        // Increase counter of consecutive times the person is not approaching
        notApproachingCount = notApproachingCount<SAMPLE_SIZE?notApproachingCount+1:SAMPLE_SIZE;
        approachingCount = 0;           // Reset counter of consecutive times the person is approaching
    } // if-else approaching == state

    // If person is too far OR not approaching for SAMPLE_SIZE consecutive readings, turns off warning
    if (treatedRange > MAXDIST || notApproachingCount >= SAMPLE_SIZE) {
        noTone(SOUNDPIN);   // Turn buzzer off
        solid(0,0,0);       // Turn LED ring off
    } else {      // Turn warning on otherwise
        /*  The program enters this section in the following conditions
         * - When the distance of person is smaller than MAXDIST, AND
         * - one of the following happens:
         *  -- The person is approaching
         *  -- OR, the person is stopped/distancing for less than SAMPLE_SIZE consecutive readings
         */
        float period = lerp(0, 100, treatedRange/MAXDIST);   // Toggle period in ms, linearly interpolated by the distance
        static unsigned long t_start = 0;     // Stores when warning state changes
        unsigned long m = millis();           // Stores when was the last time the code went through this part
      #if defined(DEBUG) || defined(DEBUG_WARNING)
      {
        Serial.print("LED delay: ");Serial.print(period);Serial.println("\tms.\t");
        Serial.print("t_start: ");Serial.print(t_start);Serial.print("\tms.\t");
        Serial.print("m: ");Serial.print(m);Serial.print("\tms.\t");
        Serial.print("m-t_start: ");Serial.print(m-t_start);Serial.println("\tms.\t");
        Serial.println();
      }
      #endif // DEBUG || DEBUG_WARNING
        if(m - t_start >= period) {     // Toggle LED ring and buzzer if the last time that that happened was more than period ms ago
            t_start = millis();
            toggleWarning();
        } // if toggle
    } // else turn warning on
  #if defined(DEBUG) || defined(DEBUG_APPROACH) || defined(DEBUG_FILTER) || defined(DEBUG_WARNING)
  {
    Serial.println("\n-----------------------------------------------\n");
  }
  #endif // DEBUG || DEBUG_APPROACH || DEBUG_FILTER || DEBUG_WARNING
} // loop



/*******************
 *  Our Functions  *
 *******************/
float lerp(float a, float b, float i) {   // Linear Interpolation from A to B controlled by i [0-1]
    return (1-i)*a + i*b;
}

void toggleWarning() {      // Toggles the warning state (turn LEDs and buzzer on and off)
    static boolean warningOn = false;    // Controls the warning intermitence
    
    if (warningOn) {
        noTone(SOUNDPIN);
        solid(0,0,0);
    } else {
        tone(SOUNDPIN, SOUNDFREQ);
        solid(255,0,0);       // Brightest red possible
    }
    warningOn = !warningOn;
}

void solid(uint8_t R, uint8_t G, uint8_t B) {     // Puts all LEDs of the ring with the same color (0,0,0 to turn off)
    for (int i = 0; i<NUMPIXELS; i++) {
        pixels.setPixelColor(i, pixels.Color(R,G,B));
    }
    pixels.show();
}

void pushToFilter(float value) {      // Add the next read value of distance in to the low pass filter kernel
    for(int i = 0; i<KERNEL_SIZE-1; i++){   // Shifts all values 1 position to the left in the array
        ranges[i] = ranges[i+1];
    }
    ranges[KERNEL_SIZE-1] = value;
}

float updateMean() {      // Updates the global var treatedRange with the new filtered value (and returns its value)
    float m = 0;
    
    for(int i = 0; i<KERNEL_SIZE; i++){     // Sums the array
        m+=ranges[i];
    }
    return treatedRange = m/KERNEL_SIZE;
}
