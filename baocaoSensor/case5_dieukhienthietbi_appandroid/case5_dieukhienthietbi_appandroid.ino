#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#include "RTClib.h"
#define Lamp 35
Servo cuaNha, remCua, cuaSo;

//Khai báo các chân của L298
int ena = 22;
int in1 = 23;
int in2 = 24;
int in3 = 25;
int in4 = 26;
int enb = 27;

void setup() {
  //#
  remCua.attach(2);
  remCua.write(0);
  cuaSo.attach(10);
  cuaSo.write(0);

  //SETUP: L298
  pinMode(ena, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enb, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  //SETUP: Lamp
  pinMode(Lamp, OUTPUT);

  //Khai báo HC-05
  Serial.begin(9600);
  Serial1.begin(4800);
}



void loop() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n'); // Đọc dữ liệu đến khi gặp ký tự xuống dòng (\n)
    
    if (input.startsWith("0")) {
      cuaSo.write(180);
    } else if (input.startsWith("1")) {
      cuaSo.write(0);
    } else if (input.startsWith("2")) {
      remCua.write(180);
    } else if (input.startsWith("3")) {
      remCua.write(0);
    } else if (input.startsWith("4")) {
      digitalWrite(ena, HIGH);
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
    } else if (input.startsWith("5")) {
      digitalWrite(ena, HIGH);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
    } else if (input.startsWith("6")) {
      digitalWrite(enb, HIGH);
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
    } else if (input.startsWith("7")) {
      digitalWrite(enb, LOW);
    } else if (input.startsWith("8")) {
      digitalWrite(Lamp, HIGH);
    } else if (input.startsWith("9")) {
      digitalWrite(Lamp, LOW);
    }
  }
}
