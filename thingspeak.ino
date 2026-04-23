#include <WiFiS3.h>
#include <ThingSpeak.h>

char ssid[]="iPhone";
char pass[]="20062009";
int status = WL_IDLE_STATUS;
WiFiClient client;
// ThingSpeak Channel Configuration
unsigned long myChannelNumber= 3309411;
const char*myWriteAPIKey = "47M03YA3CMV7FVYH";

//  PINS
const int trigPin = 9;
const int echoPin = 10;
const int moisturePin = A0;
const int buttonPin = 4;
const int redPin = 3;
const int greenPin = 6;
const int bluePin = 11;
// Thresholds
const int fullThreshold = 10;
const int mediumThreshold = 20;
const int moistureThreshold = 614;
//  DISTANCE FUNCTION
float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);

  if (duration == 0) return -1;

  return (duration * 0.034) / 2;
}
//RGB LED control
void setColor(int r, int g, int b) {
  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
}


void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  while(status!=WL_CONNECTED){
    Serial.print("Attempting to connect to SSID:");
    Serial.println(ssid);
    status= WiFi.begin(ssid, pass);
    delay(10000);
  }
  Serial.println("You're connected to the network");
  Serial.println();
  ThingSpeak.begin(client);

}

void loop() {
  float distance = getDistance();
  int moisture = analogRead(moisturePin);
  int button = digitalRead(buttonPin);
   // Bin Level Classification
  int statusValue;
  if (distance == -1){
    statusValue = -1;
  }
  else if (distance <= fullThreshold){
    statusValue = 2; // FULL
  }
  else if (distance <= mediumThreshold){
    statusValue = 1; // HALF
  }
  else{
    statusValue = 0; // EMPTY
  }
  if (distance != -1) {
    if (distance <= fullThreshold) {
    setColor(255, 0, 0);   // RED: Bin Full
  } else if (distance <= mediumThreshold) {
    setColor(0, 0, 255);   // BLUE: Medium Fill 
  } else {
    setColor(0, 255, 0);   // GREEN: Empty/Low 
  }
  }

  // Waste Classification
  int wasteValue;
  if (moisture < moistureThreshold){
    wasteValue = 1;//wet
  }
  else{
    wasteValue = 0; // DRY
  }

  //  ALERT
  int alertValue;
  if (button == LOW){
    alertValue = 1;
  }
  else{
    alertValue = 0;
  }
  //collection
  int collectionstatus;
  if (distance < fullThreshold || button== LOW){
      collectionstatus = 1;
    }
    else{
      collectionstatus = 0;
    }

  // SEND TO THINGSPEAK
  ThingSpeak.setField(1, distance);
  ThingSpeak.setField(2, statusValue);
  ThingSpeak.setField(3, wasteValue);
  ThingSpeak.setField(4, alertValue);
  ThingSpeak.setField(5, collectionstatus);

  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if (x==200){
    Serial.println("Channel upate Successful.");
    Serial.print("Field 1 Distance:");
    Serial.println(distance);
    Serial.print("Field 2 status:");
    Serial.println(statusValue);
    Serial.print("Field 3 waste:");
    Serial.println(wasteValue);
    Serial.print("Field 4 alert:");
    Serial.println(alertValue);
    Serial.print("Field 5 collectionstatus:");
    Serial.println(collectionstatus);

    Serial.println("Data was sent.");
  }
  else{
    Serial.println("Problem updating the channel. HTTP error code"+ String(x));
   
  }
  delay(15000);



}