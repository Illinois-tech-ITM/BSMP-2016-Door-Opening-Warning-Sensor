#define LEDPIN 13 // Pin where the LED for warning is hooked up
#define L1_WARNING_DIST 4 // Distance (in m) Threshold to low warning
#define L2_WARNING_DIST 3 // Distance (in m) Threshold to mid warning
#define L3_WARNING_DIST 2 // Distance (in m) Threshold to max warning
#define __time__(d) (d/343.0*2000000.0) // Convert m to us
#define __dist__(t) (t/2000000.0*343.0) // Convert us to m
#define TIMER_HIST 250 // Timer histeresys (in ms) for warning level reduction
#define SIGPIN 5 // Pin where the SIG from sonic rangefinder is plugged in
#define SAMPLES 2 // Peak Handling 
void (*warningFunc) ();
int lim = 3;


int sonicPing() {
  // Sending 1 to port SIGPIN for 5us
  pinMode(SIGPIN, OUTPUT);
  digitalWrite(SIGPIN, HIGH);
  delayMicroseconds(5);
  digitalWrite(SIGPIN, LOW);

  // Receiving duration of echo pulse (distance in ms from rangefinder)
  pinMode(SIGPIN, INPUT);
  //Serial.print("Test2 ");
  return pulseIn(SIGPIN, HIGH, 20000);

  
}

void warningHandling(int pulseDuration) {
  static int thres = SAMPLES;

 // Serial.println(("Entrou WarningHandling"));


  if (pulseDuration < __time__(L3_WARNING_DIST)) { // Warning Level 3
    thres--;
    if (lim != 3) {
      thres = SAMPLES;
    }
    if (thres == 0) {
      warningFunc = warningL3;
      Serial.println(("Fez zona 3"));
      thres = SAMPLES;
    }
    lim = 3;
    
    
  } 
  
  else if (pulseDuration < __time__(L2_WARNING_DIST)) { // Warning Level 2
    thres--;
    if (lim != 2) {
      thres = SAMPLES;
    }
    if (thres == 0) {
      warningFunc = warningL2;
      Serial.println(("Fez zona 2"));
      thres = SAMPLES;
    }
    lim = 2;
    
    
  } 
  
  else if (pulseDuration < __time__(L1_WARNING_DIST)) { // Warning Level 1
    thres--;
    if (lim != 1) {
      thres = SAMPLES;
    }
    if (thres == 0) {
      warningFunc = warningL1;
      Serial.println(("Fez zona 1"));
      thres = SAMPLES;
    }
    lim = 1;
    
    
  } 
  
  else { // No Warning
    thres--;
    if (lim != 0) {
      thres = SAMPLES;
    }
    if (thres == 0) {
      warningFunc = noWarning;
      Serial.println(("Fez zona 0"));
      thres = SAMPLES;
    }
    lim = 0;
    
    
  }
}
void noWarning() {
 // Serial.println(("Desligando LED"));
  if (digitalRead(LEDPIN) == HIGH){
    digitalWrite(LEDPIN, LOW);
  }
}

void warningL1() {
  static byte lvl = 0;
 // Serial.println(("Piscando LED devagar"));
  if (++lvl == 255) {
    lvl = 0;
    digitalWrite(LEDPIN, !digitalRead(LEDPIN));
  }  
}

void warningL2() {
  static byte lvl = 0;
 // Serial.println(("Piscando LED medio"));
  if (++lvl == 100) {
    lvl = 0;
    digitalWrite(LEDPIN, !digitalRead(LEDPIN));
  }
}

void warningL3() {
  static byte lvl = 0;
  //Serial.println(("Piscando o LED rapido"));
  if (++lvl == 50) {
    lvl = 0;
    digitalWrite(LEDPIN, !digitalRead(LEDPIN));
  }
}

void setup() {
  // put your setup code here, to run once:
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(9600);
  while (!Serial) {;}
  warningFunc = noWarning;
}

void loop() {
  // put your main code here, to run repeatedly:
  int pulseWidth = sonicPing();
  delayMicroseconds(200);

 // Serial.print("Test1 ");
  warningHandling(pulseWidth);
  warningFunc();
}

