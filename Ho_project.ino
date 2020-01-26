
#include <SPI.h>
#include <MFRC522.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F,16,2);

int s=0;

constexpr uint8_t RST_PIN = 9;     
constexpr uint8_t SS_PIN = 10;     

#include <Servo.h>
Servo myservo;
 
int pos = 40;

#define buzzer 4
#define ldrpin A3

#define smokePin A6
#define gasPin A2
#define tempPin A7
#define relayPin 6


int smokeRead, gasRead, tempRead;
float R1 = 2252;
float logR2, R2, T;
float A = 1.484778004e-03, B = 2.348962910e-04, C = 1.006037158e-07;


MFRC522 mfrc522(SS_PIN, RST_PIN);   
int lasur=0;

#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
SMSGSM sms;


int numdata;
boolean started=false;
char smsbuffer[160];
char n[20],number[20]="01671683116";


int i;

void gms(){
   Serial.begin(9600);
     Serial.println("GSM Shield testing.");
    if (gsm.begin(4800)) {
          Serial.println("\nstatus=READY");
          started=true;
     } else Serial.println("\nstatus=IDLE");

     if(started) {
         
          if (sms.SendSMS(number, "Project ready to Use")){
          Serial.println("\nSMS sent OK");
          }
          for (i = 1; i <= 20; i++)
           {
             sms.DeleteSMS(i);
           }
     }
  
}

void setup() {

  pinMode(smokePin, INPUT);
  pinMode(gasPin, INPUT);
  pinMode(tempPin, INPUT);
  pinMode(relayPin,OUTPUT);

  lcd.begin();
  lcd.backlight();

  pinMode(ldrpin,INPUT);
  myservo.attach(8); 
  myservo.write(pos);
   
  pinMode(buzzer,OUTPUT);
  
  Serial.begin(9600);                                          
  SPI.begin();                                                 
  mfrc522.PCD_Init();                                              
  Serial.println(F("Read personal data on a MIFARE PICC:"));  

   gms(); 
}

int sk=0,te=0;

void smoke(){
    //int smokeRead = analogRead(smokePin);
  //gasRead = analogRead(gasPin);
  tempRead = analogRead(tempPin);
  R2 = R1 * (1023.0 / (float)tempRead - 1.0);
  logR2 = log(R2);
  T = (1.0 / (A + B * logR2 + C * logR2 * logR2 * logR2));
  T =  T - 273.15 ;


  // Serial.println(smokeRead);
//  Serial.println(gasRead);
//  Serial.println(flameRead);
  Serial.println(T);

  if(T>50){
    te=1;
     digitalWrite(buzzer,1);
      lcd.setCursor(0,0);
         lcd.print("Hitted!!!");
    if (sms.SendSMS(number, "Over hitted your Home")){
        

         delay(2000);
         lcd.clear();

         
          }
    
  }
  else
  {
    te=0;
  }

  if (smokeRead > 120) {

    Serial.println("Smoke!!!!!");
    sk=1;
    digitalWrite(buzzer,1);

     lcd.setCursor(0,0);
         lcd.print("Smoke!!!!");
    if (sms.SendSMS(number, "smoke  in Your Home")){
        

         delay(2000);
         lcd.clear();

         
          }
    
    
  }
  else{
    sk=0;
     
  }
  

  if (gasRead > 150) {

    Serial.println("Gas!!!!!");
    digitalWrite(buzzer,1);

    if (sms.SendSMS(number, "Gash Lekage in Home")){
        

         delay(2000);
         lcd.clear();

         
          }

    
  }
  else{
    
  }


if(te==1||sk==1)
{
digitalWrite(relayPin,1);

  
}
else{
  digitalWrite(relayPin,0);
  
}
  
}

void survo(){

for (pos = 40; pos <= 140; pos += 1) { 
    myservo.write(pos);
    delay(10);
  }
  delay(1000);
  for (pos = 140; pos >= 40; pos -= 1) { 
    myservo.write(pos);             
    delay(10);                    
  }
  
}

void ldr(){

int valu =analogRead(A3);
Serial.println(valu);

 if(valu>400)
 {
  lasur=1;
 }
 else{
  lasur=0;
 }

 if(lasur==1)
 {
  
 }
 else
 {
  //send message + lcd "Someone Wall Cross"
   digitalWrite(buzzer,1);
   lcd.setCursor(0,0);
   lcd.print("UnAuthorized User");
    if (sms.SendSMS(number, "Someone Wall Cross")){
         

         delay(2000);
         lcd.clear();

         
          }
    delay(2000);
  
 }
 Serial.println(lasur);
 delay(100);
  
}



void rfid(){

  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  byte block;
  byte len;
  MFRC522::StatusCode status;

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  Serial.println(F("**Card Detected:**"));


  String re =mfrc522.PICC_DumpDetailsToSerialRE(&(mfrc522.uid)); 
  Serial.print("RE:");
  Serial.println(re);

  if(re=="246"||re=="252")
  {
    Serial.print("Authorized");
    survo();
  }
  else{
     Serial.print("UnAuthorized");
     s++;
  }

  if(s>=2){
    digitalWrite(buzzer,1);
    lcd.setCursor(0,0);
    lcd.print("UnAuthorized User");
    if (sms.SendSMS(number, "UnAuthorized User Trying to Enrty")){
         

         delay(2000);
         lcd.clear();

         
          }
    s=0;
    delay(2000);
          
    
    
  }
  
  
  
  delay(1000);
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();

 
}



void loop() {
 
  digitalWrite(buzzer,0);
  rfid();
  ldr();
  smoke();
  

  
}
