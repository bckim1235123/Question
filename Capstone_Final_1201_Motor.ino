#include <U8g2lib.h>
#include <U8x8lib.h>
#include "HX711.h"

#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif // Library

#define ENL 7
#define ENR 5
#define IN1 25

#define IN2 24
#define IN3 26

#define IN4 27

#define trig    42 //TRIG 핀 설정 (초음파 보내는 핀)
#define echo    40 //ECHO 핀 설정 (초음파 받는 핀)

#define ledB    2    
#define ledR    3
#define buzz    4

int leftLineSensor =  23;      

int rightLineSensor = 22;      

int val = 0;

int a =0 ; 
int b =0 ;



/* Arduino pin 2 -> HX711 CLK
 3 -> DOUT
 5V -> VCC
 GND -> GND*/

const int LOADCELL_DOUT_PIN = 11;
const int LOADCELL_SCK_PIN = 12;

HX711 scale;

float calibration_factor = -8500; //-7050 worked for my 440lb max scale setup

char CW [5];

U8G2_ST7920_128X64_1_HW_SPI u8g2(U8G2_R0, /* CS=*/ 10, /* reset=*/ 8);


float CW1 = 0;
float CWP = 0 ;

void forward() {      // Go

      digitalWrite(IN1, HIGH);

      digitalWrite(IN2, LOW);

      digitalWrite(IN3, HIGH);

      digitalWrite(IN4, LOW);
     
      analogWrite(ENL, val);  
        
      analogWrite(ENR, val);}



void back() {         // Back

      digitalWrite(IN1, LOW);

      digitalWrite(IN2, HIGH);

      digitalWrite(IN3, LOW);

      digitalWrite(IN4, HIGH);

      analogWrite(ENL, val);  
        
      analogWrite(ENR, val);
      }



void left() {         // Left

      digitalWrite(IN1, HIGH);

      digitalWrite(IN2, LOW);

      digitalWrite(IN3, LOW);

      digitalWrite(IN4, LOW); 
      
      analogWrite(ENL, val);
        
      analogWrite(ENR, 0);}



void right() {      // Right

      digitalWrite(IN1, LOW);

      digitalWrite(IN2, LOW);

      digitalWrite(IN3, HIGH);

      digitalWrite(IN4, LOW);
      
      analogWrite(ENL, 0);  
        
      analogWrite(ENR, val);}



void stop() {       // 정지 : 2개의 모터 모두 회전 멈춤

      digitalWrite(IN1, LOW);

      digitalWrite(IN2, LOW);

      digitalWrite(IN3, LOW);

      digitalWrite(IN4, LOW);
      
      analogWrite(ENL, 0);  
        
      analogWrite(ENR, 0);}



void setup( )  {
  Serial.begin(9600);
  pinMode(IN1, OUTPUT);     // A 모터 1

  pinMode(IN2, OUTPUT);     // A 모터 2

  pinMode(IN3, OUTPUT);     // B 모터 1

  pinMode(IN4, OUTPUT);     // B 모터 2

  pinMode(leftLineSensor, INPUT);

  pinMode(rightLineSensor, INPUT);

  pinMode(trig,OUTPUT);
  
  pinMode(echo,INPUT);
  pinMode(ENL,OUTPUT);
  
  pinMode(ENR,OUTPUT);

  pinMode(ledB,OUTPUT);
  pinMode(ledR,OUTPUT);
  pinMode(buzz,OUTPUT);  


  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  Serial.println("HX711 calibration sketch");
  Serial.println("Remove all weight from scale");
  Serial.println("After readings begin, place known weight on scale");
  Serial.println("Press + or a to increase calibration factor");
  Serial.println("Press - or z to decrease calibration factor");

  scale.set_scale();
  scale.tare(); //Reset the scale to 0

  long zero_factor = scale.read_average(); //Get a baseline reading
  Serial.print("Zero factor: "); //This can be used to remove the need to tare the scale. Useful in permanent scale projects.
  Serial.println(zero_factor);
  

  
  u8g2.begin();
  u8g2.enableUTF8Print();
  u8g2.setFont(u8g2_font_helvB10_tf); 
  u8g2.setColorIndex(1); 

}



void loop( ) {
  /*long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration*17/1000;*/
/*
  digitalWrite(trig, LOW);
  digitalWrite(echo, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
 
  unsigned long duration = pulseIn(echo, HIGH);

  float distance = duration / 29.0 / 2.0;*/
  
  float duration, distance;
  //초음파 발사 trig
  digitalWrite(trig, HIGH);
  //delay();
  digitalWrite(trig, LOW);

  //초음파가 사물 맞고 돌아온 시간 저장
  duration = pulseIn(echo, HIGH);

  //시간을 거리로 cm단위 환산
  distance = ((float)(duration * 340) / 10000) / 2;

  //시리얼 모니터에 거리 출력
  Serial.print("distance : ");
  Serial.print(distance);
  //Serial.println("cm");

  
  val = map (distance, 2, 200,75, 200);

    
  a=digitalRead(23);
  b=digitalRead(22);

 /* 
  Serial.print("sensor L : ");
  Serial.print(a);
  Serial.print(".......");
  Serial.print("sensor R :");
  Serial.println(b);
  Serial.print("distance:");
  Serial.println(distance);
  */Serial.print(".......");
  Serial.print("val:");
  Serial.println(val);

 CWP = scale.get_units()*0.453592, 3 ;
  float abs1 = abs (CWP);
  char result[8]; // Buffer big enough for 7-character float
  dtostrf(CWP, 6, 2, result); // Leave room for too large numbers!
  Serial.print("CWP :");
  Serial.println(abs1);
if (!digitalRead(leftLineSensor) && !digitalRead(rightLineSensor) && distance >5  && distance < 255 && abs1 <1) {

  forward();
  digitalWrite(ledB,HIGH);
  digitalWrite(ledR,LOW);
  digitalWrite(buzz,LOW);  
  
 }  


else if (!digitalRead(leftLineSensor) && digitalRead(rightLineSensor) && distance >5 && distance < 255 && abs1 <1) {

  left();
  digitalWrite(ledB,HIGH);
  digitalWrite(ledR,LOW);
  digitalWrite(buzz,LOW);  

 }   


else if (digitalRead(leftLineSensor) && !digitalRead(rightLineSensor)  && distance >5 && distance < 255 && abs1 <1) {

  right();
  digitalWrite(ledB,HIGH);
  digitalWrite(ledR,LOW);
  digitalWrite(buzz,LOW);
 } 


else if (distance < 10 ) {

  stop();
  digitalWrite(ledB,HIGH);
  digitalWrite(ledR,LOW);
  digitalWrite(buzz,LOW);  }

else if (abs1 > 1 ) {

  stop();
  digitalWrite(ledB,LOW);
  digitalWrite(ledR,HIGH);
  digitalWrite(buzz,HIGH);

 } 

  scale.set_scale(calibration_factor); //Adjust to this calibration factor
  
  //Serial.print("Reading: ");
  //Serial.println(scale.get_units()*0.453592, 3);
  //Serial.print(" kg"); //Change this to kg and re-adjust the calibration factor if you follow SI units like a sane person
  //Serial.print(" calibration_factor: ");
  //Serial.print(calibration_factor);
  //Serial.println();
  //Serial.print("CWValue :");
  CWP = scale.get_units()*0.453592, 3 ;
//  float abs1 = abs (CWP);
//  char result[8]; // Buffer big enough for 7-character float
//  dtostrf(CWP, 6, 2, result); // Leave room for too large numbers!
  
//  Serial.print("CWP :");
//  Serial.println(abs1);
  
  if(Serial.available())
  {
    char temp = Serial.read();
    if(temp == '+' || temp == 'a'){
      Serial.println("증가");
      calibration_factor += 10;
    }else if(temp == '-' || temp == 'z'){
      Serial.println("감소");
      calibration_factor -= 10;
    }
    
  }
/*
  u8g2.firstPage();
  do {   
    draw();
  } while( u8g2.nextPage() );
*/


}
/*
void draw(){
  CWP = scale.get_units()*0.453592, 3 ;
  float abs1 = abs (CWP);
  char result[8]; // Buffer big enough for 7-character float
  dtostrf(CWP, 6, 2, result); // Leave room for too large numbers!

  Serial.print("CWP :");
  Serial.println(result);
  readWeight();
  //readHumidity();
  
  u8g2.drawFrame(0,0,128,31);         
  u8g2.drawFrame(0,33,128,31);           
  
  u8g2.drawStr( 15, 13, "ProperWeight");   
  u8g2.drawStr( 35, 28, "2.5");   
  //u8g2.drawUTF8(70, 28, DEGREE_SYMBOL);
  u8g2.drawUTF8(76, 28, "Kg");

  u8g2.drawStr(15,46, "Current Weight");         
  u8g2.drawStr( 37, 61, result); 
  u8g2.drawStr(75,61, "Kg"); 

  if (abs1 > 1)
  {
  u8g2.drawFrame(0,0,128,31);         
  u8g2.drawFrame(0,33,128,31);
    
  u8g2.drawStr( 15, 13, "ProperWeight");   
  u8g2.drawStr( 35, 28, "1.0");   
  //u8g2.drawUTF8(70, 28, DEGREE_SYMBOL);
  u8g2.drawUTF8(76, 28, "Kg");

  u8g2.drawStr(15,46, "Warning !");         
  u8g2.drawStr( 37, 61, result); 
  u8g2.drawStr(75,61, "Kg");
  stop();
  digitalWrite(ledB,LOW);
  digitalWrite(ledR,HIGH);
  digitalWrite(buzz,HIGH);  
  
   }
  
  else {
  u8g2.drawFrame(0,0,128,31);         
  u8g2.drawFrame(0,33,128,31);           
  
  u8g2.drawStr( 15, 13, "ProperWeight");   
  u8g2.drawStr( 35, 28, "1.0");   
  //u8g2.drawUTF8(70, 28, DEGREE_SYMBOL);
  u8g2.drawUTF8(76, 28, "Kg");

  u8g2.drawStr(15,46, "Current Weight");         
  u8g2.drawStr( 37, 61, result); 
  u8g2.drawStr(75,61, "Kg");
  }
}
*/
void readWeight()
{
  CWP = scale.get_units()*0.453592, 3 ;
  float abs1 = abs (CWP);
  char result[8]; // Buffer big enough for 7-character float
  dtostrf(CWP, 6, 2, result); // Leave room for too large numbers!
  
  //Serial.print("CWP :");
  //Serial.println(CWP);
}
