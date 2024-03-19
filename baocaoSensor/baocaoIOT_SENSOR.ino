

#include <Keypad.h>
#include <Wire.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#include "RTClib.h"
int uuTien = 0;
RTC_DS1307 rtc;
//Khai báo đèn
#define Lamp 35

//Khai báo KEYPAD
const int rs = 53, en = 52, d4 = 51, d5 = 50, d6 = 49, d7 = 48;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int rs_1 = 47, en_1 = 46, d4_1 = 45, d5_1 = 44, d6_1 = 43, d7_1 = 42;
LiquidCrystal lcd_1(rs_1, en_1, d4_1, d5_1, d6_1, d7_1);
Servo cuaNha, remCua, cuaSo;

char keys[4][3] = {
  { '1', '2', '3' },
  { '4', '5', '6' },
  { '7', '8', '9' },
  { '*', '0', '#' }
};

byte rowPin[4] = { 6, 7, 8, 9 };
byte colPin[3] = { 3, 4, 5 };

String password = "123";  //password
int vitri = 0;
int loisai = 0;
int chuongBao = 12;
int total = 0;

Keypad keypad = Keypad(makeKeymap(keys), rowPin, colPin, 4, 3);
/*
  tạo đối tượng keypad và gán cho nó có tên là keypad để sử dụng!!!
  + makeKeymap(keys): Đây là một hàm sử dụng để tạo một bản đồ ký tự (keymap) cho bàn phím. Bản đồ này xác định các ký tự tương ứng với từng phím trên bàn phím số
  + rowPin chứa mảng các chân của Arduino được sử dụng để kết nối với các hàng của bàn phím số. Trong trường hợp này, có 4 hàng, và chúng được liệt kê trong mảng rowPin
  + colPin chứa mảng các chân của Arduino được sử dụng để kết nối với các cột của bàn phím số. Trong trường hợp này, có 3 cột, và chúng được liệt kê trong mảng colPin
  + 4 và 3: Hai số này đại diện cho số hàng (4) và số cột (3) trên bàn phím số. Chúng cho biết cho thư viện Keypad đang sử dụng một bàn phím số với 4 hàng và 3 cột.
*/

//Khai báo RFID
int count = 0;
char c;
String id;
int wrongAttempts = 0;  // Biến đếm số lần đọc thẻ sai

//Khai báo các chân của L298
int ena = 22;
int in1 = 23;
int in2 = 24;
int in3 = 25;
int in4 = 26;
int enb = 27;

//Khai báo RainSensor
const int rainSensor = 34;

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

//Khai báo UltraSonic
int echo = 30;
int trig = 31;
int kcToiDa = 60;                    // Khoảng cách tối đa cho phép (đơn vị: cm)
unsigned long thoiGianBatDauDo = 0;  // Biến lưu thời gian bắt đầu đo khoảng cách
bool canhBaoDaBat = false;           // Biến đánh dấu đã bật cảnh báo

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
  Serial1.begin(4800);
  remCua.attach(2);
  remCua.write(0);
  cuaSo.attach(10);
  cuaSo.write(0);

  //SETUP: LCD Hiển thị thông tin
  lcd_1.begin(20, 4);


  //SETUP: KEYPAD
  cuaNha.attach(13);
  pinMode(chuongBao, OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Moi Nhap MatKhau");
  delay(3000);
  lcd.clear();
  setLocked(true);

  //SETUP: RFID
  Serial2.begin(19200);
  Serial2.println("NHAP ID CUA THE");

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

  //SETUP: Cảm biến siêu âm
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  //SETUP: Lamp
  pinMode(Lamp, OUTPUT);

  //SETUP: RTC
  lcd_1.setCursor(0, 0);
  if (!rtc.begin()) {
    lcd_1.print("Couldn't find RTC");
    while (1)
      ;
  }

  if (!rtc.isrunning()) {
    lcd_1.print("RTC is NOT running!");
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  //SETUP: app
  Serial.begin(9600);
}



void loop() {
  //CODE: KEYPAD
  lcd.clear();
  lcd.print("Nhap PassWord: ");
  delay(100);

  char kiTuNhap = keypad.getKey();  //Lấy ký đã nhập từ bàn phím lưu vào biến kitunhap
  String key[3];

  if (kiTuNhap) {
    lcd.clear();
    lcd.print("Nhap PassWord: ");
    lcd.setCursor(vitri, 1);
    lcd.print(kiTuNhap);
    delay(200);
    if (kiTuNhap == '*' || kiTuNhap == '#') {
      vitri = 0;
      setLocked(true);
      lcd.clear();
    }

    else if (kiTuNhap == password[vitri]) {
      key[vitri] = kiTuNhap;
      vitri++;
    }

    else if (kiTuNhap != password[vitri]) {
      loisai++;
      vitri++;
    }

    if (vitri == 3) {
      if (loisai > 0) {
        total++;
        loisai = 0;
        vitri = 0;
        lcd.clear();
        lcd.setCursor(0, 2);
        lcd.print("  SAI MAT KHAU   ");
        delay(500);
        setLocked(true);
      }

      else if (vitri == 3 && loisai == 0) {
        vitri = 0;
        loisai = 0;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("CHAO MUNG BAN VE NHA");
        lcd.setCursor(2, 1);
        lcd.print(" CUA DA MO ");
        delay(2000);
        setLocked(false);
      }

      if (total == 3) {
        total = 0;
        buzzer_beep();
        delay(500);
      }
    }
  }

  //---------------------------------------------------------------------------------------------------

  //CODE: SIMULATOR RFID
  while (Serial2.available() > 0) {
    c = Serial2.read();
    count++;
    id += c;
    if (count == 12) {
      Serial2.print(id);

      if (id == "AB0123456789") {  //AB0123456987
        Serial2.println(" -> The dung, xac nhan mo cua");
        setLocked(false);
        wrongAttempts = 0;  // Reset biến đếm nếu đọc thẻ đúng
      } else {
        setLocked(true);
        Serial2.println(" -> The sai, yeu cau kiem tra lai");
        wrongAttempts++;           // Tăng biến đếm nếu đọc thẻ sai
        if (wrongAttempts == 3) {  // Nếu đọc sai đủ 3 lần
          Serial2.println("BAN DA NHAP SAI QUA 3 LAN");
          buzzer_beep();      // Gọi hàm kêu chuông
          wrongAttempts = 0;  // Reset biến đếm sau khi kêu chuông
        }
      }
      count = 0;
      id = "";
    }
  }

  //---------------------------------------------------------------------------------------------------

  //Khai báo các cảm biến

  open = digitalRead(cb_cua);                 // xem cửa đóng:1 hay mở:0
  int rainValue = digitalRead(rainSensor);    //Lấy giá trị mưa
  int lightValue = analogRead(lightSensor);   //Lấy giá trị nắng
  float doAm = dht.readHumidity();            //Lấy giá trị độ ẩm
  float nhietDo = dht.readTemperature();      //Lấy giá trị nhiệt độ
  int flameValue = digitalRead(flameSensor);  //Lấy giá trị lửa
  int gasValue = digitalRead(gasSensor);      //Lấy giá trị khí gas
  int mq135Value = analogRead(mq135Sensor);   //Lấy giá trị không khí
  int pirValue = digitalRead(pirSensor);      //Lấy giá trị chuyển động
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  unsigned long thoiGianDo = pulseIn(echo, HIGH);
  int khoangCach = int(thoiGianDo/2/29.412);

  //---------------------------------------------------------------------------------------------------

  //Xuất lên màn hình serial để kiểm tra giá trị
  //Mưa
  Serial1.println("-Rain Sensor");
  if (rainValue == 1) {
    Serial1.println("   Co mua / Thu gian phoi!");
  } else {
    Serial1.println("   Khong co mua / Keo gian phoi!");
  }
  //Chất lượng kk
  Serial1.println("-MQ135 Sensor");
  Serial1.print("   Chat luong kk: ");
  Serial1.println(mq135Value);
  //Chuyển động
  Serial1.println("-Pir Sensor");
  if (pirValue == 1) {
    Serial1.println("   Co chuyen dong!");
  } else {
    Serial1.println("   Khong co chuyen dong!");
  }
  //Gas
  Serial1.println("-Gas Sensor");
  if (gasValue == 1) {
    Serial1.println("   Khi gas ro ri!");
  } else {
    Serial1.println("   Khong khi gas ro ri!");
  }
  //Flame
  Serial1.println("-Flame Sensor");
  if (flameValue == 1) {
    Serial1.println("   Phat hien chay!");
  } else {
    Serial1.println("   Khong phat hien chay!");
  }
  //Nắng
  Serial1.println("-Light Sensor");
  Serial1.print("   Gia tri nang: ");
  Serial1.println(lightValue);
  //Nhiệt độ - độ ẩm
  Serial1.println("-DHT11");
  Serial1.print("   Gia tri do am: ");
  Serial1.println(doAm);
  Serial1.print("   Gia tri nhiet do: ");
  Serial1.println(nhietDo);
  //Cảm biến ultrasonic
  Serial1.println("-UltraSonic");
  Serial1.print("   Khoang cach truoc cua: ");
  Serial1.print(khoangCach);
  Serial1.println("cm");
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
    cuaSo.write(180);
    //Đóng cửa khi trời mưa
    if (rainValue == 1) {
      cuaSo.write(0);
    }

    //Đóng cửa khi ô nhiễm không khí
    if (mq135Value > 500) {
      cuaSo.write(0);
    }

    //Đóng cửa khi không có ai ở nhà
    if (pirValue == 1) {
      coNguoi = true;
      thoiGianBatDauDoKhongCoNguoi = millis();  // Reset thời gian khi có người ở nhà
    } else {
      coNguoi = false;
    }
    // Kiểm tra nếu không có người ở nhà trong khoảng thời gian thích hợp
    if (coNguoi == false && (millis() - thoiGianBatDauDoKhongCoNguoi >= thoiGianKhongCoNguoi)) {
      cuaSo.write(0);
    }
  }
  //CODE: KHI CỬA SỔ MỞ
  else if (open == 1) {  //Cửa đang đóng -> xét các trường hợp mở
    cuaSo.write(0);
    //mở cửa khi có khí gas
    if (gasValue == 1) {
      uuTien = 1;
      cuaSo.write(180);
    } else {
      uuTien = 0;
    }

    //Mở cửa khi phát hiện cháy
    if (flameValue == 1) {
      uuTien = 1;
      cuaSo.write(180);
    } else {
      uuTien = 0;
    }

    //Mở cửa khi độ ẩm cao
    if (doAm > 70) {
      cuaSo.write(180);
    }
  }

  //---------------------------------------------------------------------------------------------------

  //CODE: Cảm biến ULTRASONIC
  //Kiểm tra nếu có người đứng quá gần cửa trong khoảng thời gian quá lâu
  if (khoangCach < kcToiDa) {
    // Nếu người đứng quá gần cửa, kiểm tra thời gian
    if (!canhBaoDaBat) {
      // Nếu chưa bật cảnh báo, ghi lại thời gian bắt đầu
      thoiGianBatDauDo = millis();
      canhBaoDaBat = true;
    } else {
      // Nếu cảnh báo đã được bật, kiểm tra thời gian
      unsigned long thoiGianHienTai = millis();
      if (thoiGianHienTai - thoiGianBatDauDo >= 2000) {  // Thời gian quá lâu: ví dụ 2 giây
        // Bật cảnh báo
        buzzer_beep();
        canhBaoDaBat = false;  // Reset biến cảnh báo
      }
    }
  } else {
    // Nếu không có ai đứng gần cửa, reset biến cảnh báo
    canhBaoDaBat = false;
  }

  //---------------------------------------------------------------------------------------------------

  //CODE: Module thời gian thực
  DateTime now = rtc.now();  // Lấy thời gian hiện tại từ RTC
  int hour = now.hour();     // Lấy giờ hiện tại
  int minute = now.minute();
  // Hiển thị thời gian lên màn hình LCD
  lcd_1.setCursor(0, 0);
  if (now.hour() <= 9) {
    lcd_1.print("0");
    lcd_1.print(now.hour());
  } else {
    lcd_1.print(now.hour());
  }
  lcd_1.print(':');
  if (now.minute() <= 9) {
    lcd_1.print("0");
    lcd_1.print(now.minute());
  } else {
    lcd_1.print(now.minute());
  }
  lcd_1.print(':');
  if (now.second() <= 9) {
    lcd_1.print("0");
    lcd_1.print(now.second());
  } else {
    lcd_1.print(now.second());
  }
  lcd_1.print("   ");
  // Kiểm tra nếu là thời gian bật/tắt đèn
  if (hour >= 6 && hour <= 12) {
    digitalWrite(Lamp, HIGH);  // Bật đèn LED
    lcd_1.setCursor(9, 0);
    lcd_1.print("Light: ON ");

  } else {
    digitalWrite(Lamp, LOW);  // Tắt đèn LED
    lcd_1.setCursor(9, 0);
    lcd_1.print("Light: OFF");
  }

  //---------------------------------------------------------------------------------------------------

  //Hiển thị thông tin quan trọng
  //Hiển thị Nhiệt độ
  lcd_1.setCursor(0, 0);
  if (nhietDo > 30) {
    lcd_1.print("Nhiet do: ");
    lcd_1.print(nhietDo);
    lcd_1.print(" - Nong");
  } else {
    lcd_1.print("Nhiet do: ");
    lcd_1.print(nhietDo);
    lcd_1.print(" - Mat ");
  }

  //Hiển thị độ ẩm
  lcd_1.setCursor(0, 1);
  if (doAm >= 70) {
    lcd_1.print("Do am: ");
    lcd_1.print(doAm);
    lcd_1.print(" - Ko tot");
  } else {
    lcd_1.print("Do am: ");
    lcd_1.print(doAm);
    lcd_1.print(" - Tot      ");
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

  //App android 
  //Hiển thị thông tin lên app [case4]
 String mess = String(nhietDo) + " " + String(doAm) + " " + String(gasValue) + " " + String(flameValue) + " " + String(mq135Value);
  Serial.print(mess);

  //Điều khiển thiết bị trong nhà [case5]
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
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("   CANH BAO   ");
  lcd.setCursor(0, 2);
  lcd.print("   !!!!!!!!   ");
  for (int i = 0; i < 10; i++) {
    digitalWrite(chuongBao, HIGH);
    delay(1000);
    digitalWrite(chuongBao, LOW);
    delay(1000);
  }
}
