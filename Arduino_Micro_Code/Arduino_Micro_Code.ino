#define LEDPIN 13
#define DIST_THRES 15000

int sigPin = 5; // Pin where the SIG from sonic rangefinder is plugged in

void setup() {
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(9600); // Setting Serial port baud rate
  while (!Serial) {;} // Wait for the serial port be ready
}

void loop() {
  int pulseWidth = sonicPing(sigPin); // Getting distance from rangefinder

  // Printing in Serial plotter with fixed y-axis
  Serial.print(20000);
  Serial.print(F("\t"));
  Serial.print(0);
  Serial.print(F("\t"));
  Serial.print(DIST_THRES);
  Serial.print(F("\t"));
  Serial.println(pulseWidth);

  warning(pulseWidth<DIST_THRES);

  delayMicroseconds(200); 
}

int sonicPing(int port) {
  // Sending 1 to port for 5us
  pinMode(port, OUTPUT);
  digitalWrite(port, HIGH);
  delayMicroseconds(5);
  digitalWrite(port, LOW);

  // Receiving duration of echo pulse (distance in ms from rangefinder)
  pinMode(port, INPUT);
  return pulseIn(port, HIGH, 20000);
}

void warning(int warningLevel) {
  if (warningLevel > 0) { // For this moment, warningLevel>0 => Warning
    digitalWrite(LEDPIN, HIGH);

  } else if (0 == warningLevel) { // warningLevel==0 => No Warning
    digitalWrite(LEDPIN, LOW);

  } else { // Error handling
    Serial.println("ERROR: Warning Level cannot be negative");
  }

}

