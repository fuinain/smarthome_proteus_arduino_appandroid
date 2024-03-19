
#include <Keypad.h>
#include <LiquidCrystal.h>
#include <Servo.h>

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

void setup() {
  setLocked(true);
  //SETUP: KEYPAD
  cuaNha.attach(13);
  pinMode(chuongBao, OUTPUT);
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Moi Nhap MatKhau");
  delay(3000);
  lcd.clear();
  
  
  

  //SETUP: RFID
  Serial2.begin(19200);
  Serial2.println("NHAP ID CUA THE");
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
