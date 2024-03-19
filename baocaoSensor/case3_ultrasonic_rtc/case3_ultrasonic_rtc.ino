#include "RTClib.h"
#define Lamp 35
RTC_DS1307 rtc;

int ledCanhBao = 36;
int chuongBao = 12;

//Khai báo UltraSonic
int echo = 30;
int trig = 31;
int kcToiDa = 60;                    // Khoảng cách tối đa cho phép (đơn vị: cm)
unsigned long thoiGianBatDauDo = 0;  // Biến lưu thời gian bắt đầu đo khoảng cách
bool canhBaoDaBat = false;           // Biến đánh dấu đã bật cảnh báo

void setup() {
  //#
  Serial1.begin(4800);
  //SETUP: Lamp-led-buzzer
  pinMode(Lamp, OUTPUT);
  pinMode(ledCanhBao, OUTPUT);
  pinMode(chuongBao, OUTPUT);

  //SETUP: Cảm biến siêu âm
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  //SETUP: RTC
  if (!rtc.begin()) {
    Serial1.print("Couldn't find RTC");
    while (1)
      ;
  }

  if (!rtc.isrunning()) {
    Serial1.print("RTC is NOT running!");
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}



void loop() {

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  unsigned long thoiGianDo = pulseIn(echo, HIGH);
  int khoangCach = int(thoiGianDo/2/29.412);
  
  //---------------------------------------------------------------------------------------------------

  //CODE: Module thời gian thực
  DateTime now = rtc.now();  // Lấy thời gian hiện tại từ RTC
  int hour = now.hour();     // Lấy giờ hiện tại
  int minute = now.minute();

  //---------------------------------------------------------------------------------------------------

  //DS1307
  if (hour >= 17 && hour <= 20) {
    digitalWrite(Lamp, HIGH);  // Bật đèn LED
    Serial1.print("Khoang cach truoc cua: ");
    Serial1.print(khoangCach);
    Serial1.println("cm");
    Serial1.println("Troi toi - bat den");
    Serial1.println();
    delay(1000);

  } else {
    Serial1.print("Khoang cach truoc cua: ");
    Serial1.print(khoangCach);
    Serial1.println("cm");
    digitalWrite(Lamp, LOW);  // Tắt đèn LED
    Serial1.println("Troi sang - tat den");
    Serial1.println();
    delay(1000);
  }
  //Cảm biến ultrasonic

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
      if (thoiGianHienTai - thoiGianBatDauDo >= 4) {  // Thời gian quá lâu: ví dụ 4 giây
        // Bật cảnh báo
        digitalWrite(ledCanhBao, HIGH);
        buzzer_beep();
        canhBaoDaBat = false;  // Reset biến cảnh báo
        
      }
    }
  } else {
    // Nếu không có ai đứng gần cửa, reset biến cảnh báo
    canhBaoDaBat = false;
  }


  // Hiển thị thời gian lên màn hình LCD
  // lcd_1.setCursor(0, 0);
  // if (now.hour() <= 9) {
  //   lcd_1.print("0");
  //   lcd_1.print(now.hour());
  // } else {
  //   lcd_1.print(now.hour());
  // }
  // lcd_1.print(':');
  // if (now.minute() <= 9) {
  //   lcd_1.print("0");
  //   lcd_1.print(now.minute());
  // } else {
  //   lcd_1.print(now.minute());
  // }
  // lcd_1.print(':');
  // if (now.second() <= 9) {
  //   lcd_1.print("0");
  //   lcd_1.print(now.second());
  // } else {
  //   lcd_1.print(now.second());
  // }
  // lcd_1.print("   ");
  // Kiểm tra nếu là thời gian bật/tắt đèn
}

void buzzer_beep() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(chuongBao, HIGH);
    // digitalWrite(ledCanhBao, HIGH);
    delay(1000);
    digitalWrite(chuongBao, LOW);
    // digitalWrite(ledCanhBao, HIGH);
    delay(1000);
  }
}