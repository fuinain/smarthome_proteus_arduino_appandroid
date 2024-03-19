#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <DHT.h>
//Khai báo đèn
#define Lamp 35
const int rs_1 = 47, en_1 = 46, d4_1 = 45, d5_1 = 44, d6_1 = 43, d7_1 = 42;
LiquidCrystal lcd_1(rs_1, en_1, d4_1, d5_1, d6_1, d7_1);
//Khai báo DHT11
int DHTPIN = 28;
int DHTTYPE = DHT11;
DHT dht(DHTPIN, DHTTYPE);

//Khai báo MQ135 (chất lượng kk)
int mq135Sensor = A2;

//Khai báo FlameSensor
int flameSensor = 33;

//Khai báo GasSenSor
int gasSensor = 29;


void setup() {
  //SETUP: LCD Hiển thị thông tin
  lcd_1.begin(20, 4);

  //SETUP: DHT11
  dht.begin();

  //SETUP: Cảm biến chất lượng KK
  pinMode(mq135Sensor, INPUT);

  //SETUP: Cảm biến lửa
  pinMode(flameSensor, INPUT);

  //SETUP: Cảm biến gas
  pinMode(gasSensor, INPUT);

  //SETUP: Lamp
  pinMode(Lamp, OUTPUT);

  //SETUP: app
  Serial.begin(9600);
  
}



void loop() {
  //Khai báo các cảm biến
  int doAm = dht.readHumidity();              //Lấy giá trị độ ẩm
  int nhietDo = dht.readTemperature();        //Lấy giá trị nhiệt độ
  int mq135Value = analogRead(mq135Sensor);   //Lấy giá trị không khí
  int flameValue = digitalRead(flameSensor);  //Lấy giá trị lửa
  int gasValue = digitalRead(gasSensor);      //Lấy giá trị khí gas
  //CODE: App
  String mess = String(nhietDo) + " " + String(doAm) + " " + String(gasValue) + " " + String(flameValue) + " " + String(mq135Value);
  Serial.print(mess);
  //---------------------------------------------------------------------------------------------------

  //Hiển thị thông tin quan trọng
  //Hiển thị Nhiệt độ
  lcd_1.setCursor(0, 0);
  if (nhietDo > 30) {
    lcd_1.print("Nhiet do: ");
    lcd_1.print(nhietDo);
    // lcd_1.print(" - Nong");
  } else {
    lcd_1.print("Nhiet do: ");
    lcd_1.print(nhietDo);
    // lcd_1.print(" - Mat ");
  }

  //Hiển thị độ ẩm
  lcd_1.setCursor(0, 1);
  if (doAm >= 70) {
    lcd_1.print("Do am: ");
    lcd_1.print(doAm);
    // lcd_1.print(" - Ko tot");
  } else {
    lcd_1.print("Do am: ");
    lcd_1.print(doAm);
    // lcd_1.print(" - Tot      ");
  }

  //Hiển thị chất lượng kk
  lcd_1.setCursor(0, 2);
  if (mq135Value >= 0 && mq135Value < 50) {
    lcd_1.print("AQI: ");
    lcd_1.print(mq135Value);
    lcd_1.print(" -  Tot       ");
  } else if (mq135Value >= 51 && mq135Value <= 100) {
    lcd_1.print("AQI: ");
    lcd_1.print(mq135Value);
    lcd_1.print(" -  Trung binh");
  } else if (mq135Value >= 101 && mq135Value <= 150) {
    lcd_1.print("AQI: ");
    lcd_1.print(mq135Value);
    lcd_1.print(" -  Xau       ");
  } else if (mq135Value >= 151 && mq135Value <= 200) {
    lcd_1.print("AQI: ");
    lcd_1.print(mq135Value);
    lcd_1.print(" -  Kem       ");
  } else if (mq135Value >= 201 && mq135Value <= 300) {
    lcd_1.print("AQI: ");
    lcd_1.print(mq135Value);
    lcd_1.print(" -  Rat kem   ");
  } else {
    lcd_1.print("AQI: ");
    lcd_1.print(mq135Value);
    lcd_1.print(" -  Nguy hai  ");
  }

  //Hiển thị gas - flame
  lcd_1.setCursor(0, 3);
  if (gasValue == 1) {
    lcd_1.print("Gas: Yes");
    lcd_1.print("/");
  } else {
    lcd_1.print("Gas: No ");
    lcd_1.print("/");
  }
  lcd_1.setCursor(8, 3);
  if (flameValue == 1) {
    lcd_1.print("/");
    lcd_1.print("flame: Yes");
  } else {
    lcd_1.print("/");
    lcd_1.print("flame: No ");
  }
  delay(500);
  
}
