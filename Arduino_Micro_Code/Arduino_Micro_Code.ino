int sigPin = 5; // Pin where the SIG from sonic rangefinder is plugged in
int pulseWidth = 0; // Duration in milliseconds of the sonic rangefinder (the lesser (but >0) the closer to the rangefinder)

void setup() {
  Serial.begin(9600); // Setting Serial port baud rate
  while(!Serial){;} // Wait for the serial port be ready
}

void loop() {
  pulseWidth = sonicPing(sigPin); // Getting distance from rangefinder

  // Printing in Serial plotter with fixed y-axis
  Serial.print(20000);
  Serial.print("\t");
  Serial.print(0);
  Serial.print("\t");
  Serial.println(pulseWidth);
  
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

