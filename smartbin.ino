#include <Servo.h>
//  PINS
const int trigPin = 9;       // Internal Ultrasonic (Fill Level)
const int echoPin = 10;
const int trigPin_2 = 12;    // External Ultrasonic (Proximity)
const int echoPin_2 = 13;
const int pirPin = 7;        // Motion Sensor
const int servoPin = 5;      // Lid Actuator
const int moisturePin = A0;  // Moisture Classification
const int buttonPin = 4;     // Manual Alert (INPUT_PULLUP)
const int redPin = 3;        // Status RGB: RED
const int greenPin = 6;      // Status RGB: GREEN
const int bluePin = 11;      // Status RGB: BLUE

Servo lidServo;

// THRESHOLDS
const int fullThreshold = 10;     // Bin is FULL if distance <= 10cm
const int mediumThreshold = 20;   // Bin is MEDIUM if distance <= 20cm
const int moistureThreshold = 614;// ADC threshold for WET vs DRY
const long openDuration = 5000;   // Lid stay-open time (5 seconds)
const int proximitythreshold = 20; // Detects motion if  distance < 20cm

// SYSTEM VARIABLES
unsigned long lastLidMillis = 0;  
unsigned long lastSerialMillis = 0;
const long serialInterval = 1000; // Log data every 1 second
int binState = 0;                 // 0 = Closed, 1 = Open
float lastValidDistance = 25.0;   // Holds last right reading when lid is open


 // SET RGB COLOR
 
void setColor(int r, int g, int b) {
  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}


 // GET DISTANCE (HC-SR04) 
 // calculates distance based on the speed of sound.
 
float getDistance(int trigger, int echo) {
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  
  long duration = pulseIn(echo, HIGH, 30000); 
  if (duration <= 0) return 999; // Return "Out of Range" if no echo
  return (duration * 0.034) / 2;
}

void setup() {
  Serial.begin(9600);
  
  // Initialize Pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(trigPin_2, OUTPUT);
  pinMode(echoPin_2, INPUT);
  pinMode(pirPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP); // HIGH when open, LOW when pressed
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  // Initialize Actuators
  lidServo.attach(servoPin);
  lidServo.write(0); // Ensure lid starts in the CLOSED position

}

void loop() {
  unsigned long currentMillis = millis();
  // If lid is OPEN (State 1), the lid-mounted sensor is angled away.
  float distance;
  if (binState == 0) {
    distance = getDistance(trigPin, echoPin);
    lastValidDistance = distance; // Update memory only when lid is closed
  } else {
    distance = lastValidDistance; // Hold last known reading while lid is open
  }

  delay(50); // Crosstalk Prevention
  
  float proximity = getDistance(trigPin_2, echoPin_2); 
  int motion = digitalRead(pirPin);
  int moisture = analogRead(moisturePin);
  int buttonActive = digitalRead(buttonPin); 

  // 2. LID CONTROL  
  if (binState == 0) {
    //  Motion detected AND user is in proximity AND bin is not full
    if (motion == HIGH && proximity < proximitythreshold && lastValidDistance > fullThreshold) {
      Serial.println(">>> ACTION: OPENING_LID");
      lidServo.write(90);
      lastLidMillis = currentMillis;
      binState = 1;
    }
  }
  else if (binState == 1) {
    // Reset timer if user is still detected
    if (motion == HIGH || proximity < proximitythreshold) {
      lastLidMillis = currentMillis; 
    }
    
    //  Close after timer expires
    if (currentMillis - lastLidMillis >= openDuration) {
      Serial.println(">>> ACTION: CLOSING_LID");
      lidServo.write(0);
      binState = 0;
    }
  }

  //  Visual representation of fill level
  if (lastValidDistance <= fullThreshold) {
    setColor(255, 0, 0);   // RED: Bin Full
  } else if (lastValidDistance <= mediumThreshold) {
    setColor(0, 0, 255);   // BLUE: Medium Fill 
  } else {
    setColor(0, 255, 0);   // GREEN: Empty/Low 
  }

  // DATA LOGGING
  if (currentMillis - lastSerialMillis >= serialInterval) {
    lastSerialMillis = currentMillis;

    String wasteType;
    if (moisture < moistureThreshold) {
      wasteType = "WET";
    } else {
      wasteType = "DRY";
    }
    
    // Alerts if physically full OR if a user manually presses the button
    String collectionstatus;
    if (distance < fullThreshold || buttonActive == 0){
      collectionstatus = "YES";
    }
    else{
      collectionstatus = "NO";
    }
    int alertValue;
    if (buttonActive == 0) {
    alertValue = 1;
    } else {
    alertValue = 0;
    }


    Serial.print("Fill: ");   Serial.print(lastValidDistance);
    Serial.print(" | Type: ");   Serial.print(wasteType);
    Serial.print(" | Hand: ");   Serial.print(proximity);
    Serial.print(" | Motion: "); Serial.print(motion);
    Serial.print(" | Alert: ");  Serial.print(alertValue);
    Serial.print(" | status: "); Serial.println(collectionstatus);
  }
}
