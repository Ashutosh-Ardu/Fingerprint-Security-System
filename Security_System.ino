#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal.h>
#include <Servo.h>

/*
 Link : https://electropeak.com/learn/interfacing-fpm10a-as608-optical-fingerprint-reader-sensor-module-with-arduino/
 */

//initializations
Servo myservo;
const int rs=12,en=11,d4=5,d5=4,d6=3,d7=2;
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);
int pos = 0;
int buzzer = 13;
int red=10;
int blue=A0;
int green=7;
volatile int finger_status = -1;

SoftwareSerial mySerial(8, 9); // TX/RX on fingerprint sensor

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup()  
{
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  
  Serial.println("\n\nAdafruit finger detect test");
  lcd.begin(16,2);
  lcd.print("Welcome");
  myservo.attach(6); 
  myservo.write(pos); 
  pinMode(buzzer, OUTPUT);
  pinMode(red,OUTPUT);
  pinMode(blue,OUTPUT);
  pinMode(green,OUTPUT);
  
  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  finger.getTemplateCount();
  Serial.print("Sensor contains "); Serial.print(finger.templateCount); Serial.println(" templates");
  Serial.println("Waiting for valid finger...");
}

void loop()                     // run over and over again
{
  finger_status = getFingerprintIDez();
  analogWrite(red,255);
   analogWrite(blue,0);
   analogWrite(green,0); 
  if (finger_status!=-1 and finger_status!=-2){
    Serial.println("");
  } else{
    if (finger_status==-2){
      for (int ii=0;ii<5;ii++){
        Serial.println("Not a Match");
        lcd.clear();
        lcd.print("Not a Match");
      }
    }
    else{
      Serial.println("Not a Match");
      lcd.clear();
      lcd.print("Insert");
    }
  }
  delay(50);            //don't ned to run this at full speed.
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
//  if (p!=2){
//    Serial.println(p);
//  }
  if (p != FINGERPRINT_OK)  return -1;
  
  p = finger.image2Tz();
//  if (p!=2){
//    Serial.println(p);
//  }
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -2;
  
  // found a match!
  Serial.println("Found a Match");
  lcd.clear();
  lcd.print("Match Accepted");
  delay(500);
  Serial.print("ID # "); Serial.print(finger.fingerID); 
  Serial.print("  with a confidence level of "); Serial.println(finger.confidence);

  lcd.clear();
  lcd.print("Opening...");
  
  for(int k = 0;k < 3;k++){
    analogWrite(red,0);
   analogWrite(blue,0);
   analogWrite(green,255); 
    tone(buzzer,100);
   delay(1000);
    noTone(buzzer);
    analogWrite(red,0);
  analogWrite(blue,0);
  analogWrite(green,0);
    delay(1000);
  }

  
  
  analogWrite(red,0);
   analogWrite(blue,0);
   analogWrite(green,255); 
  
  for (pos = 0; pos <= 90; pos += 1) {
  myservo.write(pos);              
  delay(5);                       
  }
  
  delay(5000);

  lcd.clear();
  lcd.print("Closing...");
  
  analogWrite(red,255);
  analogWrite(blue,0);
  analogWrite(green,0);
    
  for(int k = 0;k < 3;k++){
    analogWrite(red,255);
   analogWrite(blue,0);
   analogWrite(green,0); 
    tone(buzzer,100);
   delay(1000);
    noTone(buzzer);
    analogWrite(red,0);
  analogWrite(blue,0);
  analogWrite(green,0);
    delay(1000);
  }
  
  
  for (pos = 90; pos >= 0; pos -= 1) {
  myservo.write(pos);              
  delay(5);                       
  }

  lcd.clear();
  lcd.print("Closed");
  delay(500);
  
  return finger.fingerID; 
}
