#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#include "RTClib.h"
#define Lamp 35
int uuTien = 0;
RTC_DS1307 rtc;
Servo cuaNha, remCua, cuaSo;
int chuongBao = 12;
//Khai báo các chân của L298
int ena = 22;
int in1 = 23;
int in2 = 24;
int in3 = 25;
int in4 = 26;
int enb = 27;

//Khai báo RainSensor
int rainSensor = 34;

//Khai báo LightSensor
int lightSensor = A1;

//Khai báo DHT11
int DHTPIN = 28;
int DHTTYPE = DHT11;
DHT dht(DHTPIN, DHTTYPE);

//Khai báo FlameSensor
int flameSensor = 33;

//Khai báo GasSenSor
int gasSensor = 29;

//Khai báo PirSensor
int pirSensor = 32;
unsigned long thoiGianKhongCoNguoi = 2000;
unsigned long thoiGianBatDauDoKhongCoNguoi = 0;
bool coNguoi = false;

//Khai báo MQ135 (chất lượng kk)
int mq135Sensor = A2;

//Khai báo cảm biến từ
int cb_cua = 11;
int open;

void setup() {
  //#
  cuaNha.attach(13);
  setLocked(true);
  Serial1.begin(4800);
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

  //SETUP: RainSensor
  pinMode(rainSensor, INPUT);
  digitalWrite(ena, LOW);

  //SETUP: LightSensor
  pinMode(lightSensor, INPUT);

  //SETUP: DHT11
  dht.begin();

  //SETUP: Cảm biến từ
  pinMode(cb_cua, INPUT_PULLUP);

  //SETUP: Cảm biến lửa
  pinMode(flameSensor, INPUT);

  //SETUP: Cảm biến gas
  pinMode(gasSensor, INPUT);

  //SETUP: Cảm biến chất lượng KK
  pinMode(mq135Sensor, INPUT);

  //SETUP: Cảm biến chuyển động
  pinMode(pirSensor, INPUT);

  //SETUP: ĐÈN
  pinMode(Lamp, OUTPUT);
  pinMode(chuongBao, OUTPUT);

}



void loop() {
  
  open = digitalRead(cb_cua);               // xem cửa đóng:1 hay mở:0
  int rainValue = digitalRead(rainSensor);    //Lấy giá trị mưa
  int lightValue = analogRead(lightSensor);   //Lấy giá trị nắng
  float doAm = dht.readHumidity();            //Lấy giá trị độ ẩm
  float nhietDo = dht.readTemperature();      //Lấy giá trị nhiệt độ
  int flameValue = digitalRead(flameSensor);  //Lấy giá trị lửa
  int gasValue = digitalRead(gasSensor);      //Lấy giá trị khí gas
  int mq135Value = analogRead(mq135Sensor);   //Lấy giá trị không khí
  int pirValue = digitalRead(pirSensor);      //Lấy giá trị chuyển động

  //---------------------------------------------------------------------------------------------------

  //Xuất lên màn hình serial để kiểm tra giá trị
  //Mưa
  Serial1.println("1.Rain Sensor");
  if (rainValue == 1) {
    Serial1.println("   Co mua / Thu gian phoi!");
  } else {
    Serial1.println("   Khong co mua / Keo gian phoi!");
  }
  //Chất lượng kk
  Serial1.println("2.MQ135 Sensor");
  Serial1.print("   Chat luong kk: ");
  Serial1.println(mq135Value);
  //Chuyển động
  Serial1.println("3.Pir Sensor");
  if (pirValue == 1) {
    Serial1.println("   Co chuyen dong!");
  } else {
    Serial1.println("   Khong co chuyen dong!");
  }
  //Gas
  Serial1.println("4.Gas Sensor");
  if (gasValue == 1) {
    Serial1.println("   Khi gas ro ri!");
  } else {
    Serial1.println("   Khong khi gas ro ri!");
  }
  //Flame
  Serial1.println("5.Flame Sensor");
  if (flameValue == 1) {
    Serial1.println("   Phat hien chay!");
  } else {
    Serial1.println("   Khong phat hien chay!");
  }
  //Nắng
  Serial1.println("6.Light Sensor");
  Serial1.print("   Gia tri nang: ");
  Serial1.println(lightValue);
  //Nhiệt độ - độ ẩm
  Serial1.println("7.DHT11");
  Serial1.print("   Gia tri do am: ");
  Serial1.println(doAm);
  Serial1.print("   Gia tri nhiet do: ");
  Serial1.println(nhietDo);
  Serial1.println();
  delay(700);

  //---------------------------------------------------------------------------------------------------

  //CODE: RainSensor:
  digitalWrite(ena, LOW);
  if (rainValue == 0) {  //khong mua
    digitalWrite(ena, HIGH);
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  } else if (rainValue == 1) {  //co mua
    digitalWrite(ena, HIGH);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }

  //---------------------------------------------------------------------------------------------------

  //CODE: DHT11: Nhiệt độ cao -> mở quạt trong nhà !
  if (nhietDo >= 30) {
    digitalWrite(enb, HIGH);
    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);
  } else {
    digitalWrite(enb, LOW);
  }

  //---------------------------------------------------------------------------------------------------

  //CODE: LightSensor: Nắng -> kéo rèm
  if (lightValue > 500) {
    remCua.write(180);
  } else {
    remCua.write(0);
  }

  //---------------------------------------------------------------------------------------------------

  //CODE: TRƯỜNG HỢP KHI CỬA SỔ ĐÓNG
  if (open == 0) {  //Cửa đang mở -> xét các trường hợp đóng
    digitalWrite(Lamp, HIGH);
    cuaSo.write(180);
    //Đóng cửa khi trời mưa
    if (rainValue == 1) {
      if(uuTien==0){
        cuaSo.write(0);
      }
    }

    //Đóng cửa khi ô nhiễm không khí
    if (mq135Value > 500) {
      if(uuTien==0){
        cuaSo.write(0);
      }
    }

    //Đóng cửa khi không có ai ở nhà
    if (pirValue == 1) {
      coNguoi = true;
      thoiGianBatDauDoKhongCoNguoi = millis();  // Reset thời gian khi có người ở nhà
    }else{
      coNguoi = false;
    }
    // Kiểm tra nếu không có người ở nhà trong khoảng thời gian thích hợp
    if ( coNguoi == false && (millis() - thoiGianBatDauDoKhongCoNguoi >= thoiGianKhongCoNguoi)) {
      cuaSo.write(0);
      digitalWrite(Lamp, LOW);
    }
  }
  //CODE: KHI CỬA SỔ MỞ
  else if (open == 1) {  //Cửa đang đóng -> xét các trường hợp mở
    cuaSo.write(0);
    setLocked(true);
    //mở cửa khi có khí gas
    if (gasValue == 1) {
      uuTien = 1;
      cuaSo.write(180);
      setLocked(false);
      buzzer_beep();
    } else {
      uuTien = 0;
    }

    //Mở cửa khi phát hiện cháy
    if (flameValue == 1) {
      uuTien = 1;
      cuaSo.write(180);
      setLocked(false);
      buzzer_beep();
    } else {
      uuTien = 0;
    }

    //Mở cửa khi độ ẩm cao
    if (doAm > 70) {
      cuaSo.write(180);
    }
  }

}


void setLocked(bool locked) {
  if (locked) {
    // Cửa cần được khóa
    cuaNha.write(0);
  } else {
    // Cửa cần được mở
    cuaNha.write(180);
    delay(3000);
    cuaNha.write(0);
  }
}

void buzzer_beep() {
  for (int i = 0; i < 10; i++) {
    digitalWrite(chuongBao, HIGH);
    // digitalWrite(ledCanhBao, HIGH);
    delay(1000);
    digitalWrite(chuongBao, LOW);
    // digitalWrite(ledCanhBao, HIGH);
    delay(1000);
  }
}
