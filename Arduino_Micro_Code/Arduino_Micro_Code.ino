#define LEDPIN 13 // Pin where the LED for warning is hooked up
#define L1_WARNING_DIST 5 // Distance (in m) Threshold to low warning
#define L2_WARNING_DIST 2.5 // Distance (in m) Threshold to mid warning
#define L3_WARNING_DIST 1 // Distance (in m) Threshold to max warning
#define __time__(d) (d/343.0*2000000.0) // Convert m to us
#define __dist__(t) (t/2000000.0*343.0) // Convert us to m
#define TIMER_HIST 250 // Timer histeresys (in ms) for warning level reduction
#define SIGPIN 5 // Pin where the SIG from sonic rangefinder is plugged in

void (*warningFunc) ();

void setup() {
  pinMode(LEDPIN, OUTPUT); // Setting warning pin
  Serial.begin(9600); // Setting Serial port baud rate
  while (!Serial) {;} // Wait for the serial port be ready
}

void loop() {
  int pulseWidth = sonicPing(); // Getting distance from rangefinder

  // Printing in Serial plotter with fixed y-axis
  Serial.print(__dist__(20000));
  Serial.print(F("\t"));
  Serial.print(0);
  Serial.print(F("\t"));
  Serial.print(L1_WARNING_DIST);
  Serial.print(F("\t"));
  Serial.print(L2_WARNING_DIST);
  Serial.print(F("\t"));
  Serial.print(L3_WARNING_DIST);
  Serial.print(F("\t"));
  Serial.println(__dist__(pulseWidth));

  warningHandling(pulseWidth);
  warningFunc();

  delayMicroseconds(200); 
}

int sonicPing() {
  // Sending 1 to port SIGPIN for 5us
  pinMode(SIGPIN, OUTPUT);
  digitalWrite(SIGPIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(SIGPIN, LOW);

  // Receiving duration of echo pulse (distance in ms from rangefinder)
  pinMode(SIGPIN, INPUT);
  return pulseIn(SIGPIN, HIGH, 20000);
}

void warningHandling(int pulseDuration) {

  if (pulseDuration < __time__(L3_WARNING_DIST)) { // Warning Level 3
    warningFunc = warningL3;
    
  } else if (pulseDuration < __time__(L2_WARNING_DIST)) { // Warning Level 2
    warningFunc = warningL2;
    
  } else if (pulseDuration < __time__(L1_WARNING_DIST)) { // Warning Level 1
    warningFunc = warningL1;

  } else { // No Warning
    warningFunc = noWarning;
    
  }

}

void noWarning() {
  if (digitalRead(LEDPIN) == HIGH){
    digitalWrite(LEDPIN, LOW);
  }
}

void warningL1() {
  static byte lvl = 0;
  analogWrite(LEDPIN, lvl++);  
}

void warningL2() {
  static byte lvl = 0;
  lvl += 15;
  analogWrite(LEDPIN, lvl);
}

void warningL3() {
  if (digitalRead(LEDPIN) == LOW){
    digitalWrite(LEDPIN, HIGH);
  }
}

