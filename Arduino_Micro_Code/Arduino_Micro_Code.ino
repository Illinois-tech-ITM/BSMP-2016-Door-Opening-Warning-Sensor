#define LEDPIN 13 // Pin where the LED for warning is hooked up
#define L1_WARNING_DIST 18000 // Distance Threshold to low warning
#define L2_WARNING_DIST 12000 // Distance Threshold to mid warning
#define L3_WARNING_DIST 8000 // Distance Threshold to max warning
#define TIMER_HIST 250 // Timer histeresys for warning level reduction
#define SIGPIN 5 // Pin where the SIG from sonic rangefinder is plugged in

void setup() {
  pinMode(LEDPIN, OUTPUT); // Setting warning pin
  Serial.begin(9600); // Setting Serial port baud rate
  while (!Serial) {;} // Wait for the serial port be ready
}

void loop() {
  int pulseWidth = sonicPing(); // Getting distance from rangefinder

  // Printing in Serial plotter with fixed y-axis
  Serial.print(20000);
  Serial.print(F("\t"));
  Serial.print(0);
  Serial.print(F("\t"));
  Serial.print(L1_WARNING_DIST);
  Serial.print(F("\t"));
  Serial.print(L2_WARNING_DIST);
  Serial.print(F("\t"));
  Serial.print(L3_WARNING_DIST);
  Serial.print(F("\t"));
  Serial.println(pulseWidth);

  warningHandling(pulseWidth);

  delayMicroseconds(200); 
}

int sonicPing() {
  // Sending 1 to port for 5us
  pinMode(SIGPIN, OUTPUT);
  digitalWrite(SIGPIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(SIGPIN, LOW);

  // Receiving duration of echo pulse (distance in ms from rangefinder)
  pinMode(SIGPIN, INPUT);
  return pulseIn(SIGPIN, HIGH, 20000);
}

void warningHandling(int pulseDuration) {
  if (pulseDuration > L1_WARNING_DIST) { // Warning level 1
    digitalWrite(LEDPIN, HIGH);

  } else { // No Warning
    digitalWrite(LEDPIN, LOW);

  }

}

