/*
 * Hardware setup:
 * 
 */

// establish pin constants
long ConsoleBps = 19200;
const int sensorTrigPin = 12;          // sensorTrigPin tells distance sensor to start/stop a reading
const int sensorEchoPin = 13;          // sensorEchoPin is where distance sensor provides feedback
const int transistorGatePin = 39;      // transistorGatePin will flip the transistor to power the pneumatic solenoid valve's 12v power
const int minTriggerDistance = 10;      // minimum distance, inches,  object must be away in order to trigger
const int maxTriggerDistance = 72;      // maximum distance, inches, object must be away in order to trigger
const int risingDelayMillis = 15000;   // risingDelayMillis is number of millis to hold body up
const int loweringDelayMillis = 15000; // loweringDelayMillis is number of millis to allow body to lower before restarting loop
const int requiredHitCount = 2;       // number of matching hits from distance sensor to trigger rise
const int soundTriggerPin = 30;       // soundTriggerPin is the pin # linked to the sound card, bringing it LOW turns on sound

// establish variables for ultrasonic distance sensor
long duration, inches, countOfHits, counter;

boolean serialOn = false; //sometimes writing to Serial hangs board, so this flag turns off writing to Serial in log()
#define RLED RED_LED
#define GLED GREEN_LED
#define BLED BLUE_LED

void setup() {
  // initialize serial communication bits per second:
  Serial.begin(ConsoleBps); 

  log("Setting output on transistorGatePin ()");
  pinMode(transistorGatePin, OUTPUT);

  // ensure body is down/lowered - power off transistor (and thus solenoid)
  digitalWrite(transistorGatePin, LOW);      
  log("transistorGatePin set to LOW voltage - ensuring body is down/lowered");

 //setup sensor pins
  pinMode(sensorTrigPin, OUTPUT);
  log("Sensor Trig pin set to output");
  pinMode(sensorEchoPin, INPUT);
  log("Sensor Echo pin set to input");

  // setup sound pin and ensure sound is off to start
  pinMode(soundTriggerPin, OUTPUT);
  digitalWrite(soundTriggerPin, HIGH);

  
  log("Finished setup()...");
}

void loop() {
  digitalWrite(RLED, HIGH);
  counter++;
  countOfHits = 0;
  inches = getSensorDistance();
  log("Run 1: " + String(inches) + "in");
  if(inches > minTriggerDistance && inches < maxTriggerDistance) {
      countOfHits = countOfHits+1;
  }
  delay(1);
  inches = getSensorDistance();
  log("Run 2: " + String(inches) + "in");
  if(inches > minTriggerDistance && inches < maxTriggerDistance) {
      countOfHits = countOfHits+1;
  }
  delay(1);
  inches = getSensorDistance();
  log("Run 3: " + String(inches) + "in");
  if(inches > minTriggerDistance && inches < maxTriggerDistance) {
      countOfHits = countOfHits+1;
  }
  
  // print out distance to console
  log("Count of hits: " + String(countOfHits));
  
//   delay(2000);

 if(countOfHits >= requiredHitCount )
 {
   digitalWrite(RLED, LOW);
   digitalWrite(BLED, HIGH);
   //raise body by triggering transistor (which powers solenoid)
   log("Raising body...");
   digitalWrite(transistorGatePin, HIGH);     // powers transistor and an LED so we can see it happen

   //turn on sound - should play only once
   digitalWrite(soundTriggerPin, HIGH); //force HIGH just to be sure it is off before turning on
   delay(10);                           //slight delay to allow board to register the HIGH state
   digitalWrite(soundTriggerPin, LOW);  //trigger the sound
   delay(500);                          //give time for the pin to trigger
   digitalWrite(soundTriggerPin, HIGH); //turn off sound - pin only needs to be triggered for a moment for sound to play
   
   //stay in this state for 20 seconds
   delay(risingDelayMillis);
   
   //lower body
   log("Lowering body...");
   digitalWrite(transistorGatePin, LOW);      // power off transistor (and thus solenoid)


   // delay long enough to allow body to lower and ready for next go...
   delay(loweringDelayMillis);
   digitalWrite(BLED, LOW);
 }  
   digitalWrite(RLED, LOW);
   digitalWrite(GLED, HIGH);
   delay(3000);
   digitalWrite(GLED, LOW);
}

long getSensorDistance()
{
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:  
  digitalWrite(sensorTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(sensorTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensorTrigPin, LOW);

  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  duration = pulseIn(sensorEchoPin, HIGH);

  // convert the time into a distance
  return microsecondsToInches(duration);
}

long microsecondsToInches(long microseconds)
{
// According to Parallax's datasheet for the PING))), there are
// 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
// second). This gives the distance travelled by the ping, outbound
// and return, so we divide by 2 to get the distance of the obstacle.
// See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
return microseconds / 74 / 2;
}

void log(String logLine)
{
  if(serialOn) Serial.println(String(millis()) + ": " + logLine);
}

