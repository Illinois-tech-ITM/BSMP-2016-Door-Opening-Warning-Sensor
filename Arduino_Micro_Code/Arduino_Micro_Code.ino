/*
  Filters:
  * NONE (default): No filtering
  * MEDIAN: Take the median of a sample
  * HIGHEST_MODE: Take the mode of a sample. In case more than
  one mode, the highest one is returned
  * LOWEST_MODE: Take the mode of a sample. In case more than
  one mode, the lowest one is returned
  * BEST: Take the mode of a sample. In case more than one
  mode is found, the median is returned
  * SIMPLE: Continue reading until sample_size consecutive readings
  are issued
*/
#include "Maxbotix.h"
#define SONIC_SENSOR_PIN 8
#define SOUNDPIN A4
#define LEDPIN A3
#define MAXDIST 200

Maxbotix rangeSensorPW(SONIC_SENSOR_PIN, Maxbotix::PW, Maxbotix::LV, Maxbotix::BEST);

float lerp(float a, float b, float i);

void setup()
{
  Serial.begin(9600);
  pinMode(SOUNDPIN, OUTPUT);
  pinMode(LEDPIN, OUTPUT);
}

void loop()
{
  float range = rangeSensorPW.getRange();
  if (range > MAXDIST)
  {
    noTone(SOUNDPIN);
    digitalWrite(LEDPIN, LOW);
  }
  else 
  {
    float freq = lerp(1760, 55, range/MAXDIST);
    tone(SOUNDPIN, freq);
    
    float freq2 = lerp(1, 1000, range/MAXDIST);
    static unsigned long t_start = 0;
    unsigned long m = millis();
    
    if(m - t_start >= freq2) {
      digitalWrite(LEDPIN, !digitalRead(LEDPIN));
      t_start = millis();
    }
  }
}

float lerp(float a, float b, float i)
{
  return (1-i)*a + i*b;
}
