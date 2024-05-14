#define sensor 5

#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Khai bao Cam bien nhiet do, do am
const int DHTPIN = 4; // Đọc dữ liệu từ DHT11 ở chân 4 trên mạch Arduino
const int DHTTYPE = DHT11; // Khai báo loại cảm biến, có 2 loại là DHT11 và DHT22
DHT dht(DHTPIN, DHTTYPE);

volatile int but_set_module = 2, type_module = 0;  //0:auto(default) 1:manual
volatile int but_format_time = 3, type_time = 0; // 0:24h(default) 1:12h am-pm
int ButUp = 1;
int ButDown = 0;
bool sensorState = false; // Biến để theo dõi trạng thái của chân sensor
unsigned long lastChangeTime = 0; // Biến để lưu thời điểm cuối cùng chuyển đổi trạng thái của chân sensor

int gio = 0, phut = 0, giay = 0;
int dem = 0, gtcong, gttru, gtmenu;

void setup() {
  Serial.begin(9600);
  dht.begin(); // bật cảm biến
  lcd.init(); // thiết lập các cài đặt cho màn hình LCD
  lcd.backlight(); // bật đèn nền
  if (!rtc.begin()) {
    lcd.print("Couldn't find rtc");
    while (1);
  }

  if (!rtc.isrunning()) {
    lcd.print("RTC is not running!");
  }

  //điều chỉnh thời gian của RTC khớp với thời gian hiện tại của hệ thống
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 

  // button transform module auto/manual
  pinMode(but_set_module, INPUT);
  attachInterrupt(0, trans_module, FALLING);

  // button transform type time display
  pinMode(but_format_time, INPUT);

  // button transform type time display
  pinMode(but_format_time, INPUT);
  attachInterrupt(1, trans_type, FALLING);

  // Button Up-Down
  pinMode(ButUp, INPUT);
  pinMode(ButDown, INPUT);
  // Sound Sensor
  pinMode(sensor, INPUT);
}

void trans_module() {
  type_module = !type_module; // Chuyển đổi giữa auto và manual
}

void trans_type(){
  type_time = !type_time; // Chuyển đổi giữa 24h và 12h
}

void HienThi() {   //hàm để hiển thị thời gian

  if (type_time == 1) {
    // 12h AM/PM
    lcd.setCursor(0, 0);
    lcd.print("  ");
    if (gio == 0 || gio == 12) {
      lcd.print("12");
    } else {
      lcd.print(gio % 12);
    }
  } else {
    // 24h
    lcd.setCursor(2, 0);
    lcd.print("  ");
    if (gio <= 9) {
      lcd.print("0");
      lcd.print(gio);
    } else {
      lcd.print(gio);
    }
  }
  //Hiển thị phút
  lcd.print(":");

  if (phut <= 9) {
    lcd.print("0");
    lcd.print(phut);
  } else {
    lcd.print(phut);
  }

  lcd.print(":");
  //Hiển thị giây
  if (giay <= 9) {
    lcd.print("0");
    lcd.print(giay);
  } else {
    lcd.print(giay);
  }
  //Hiển thị hậu tố
  if (type_time == 1) {
    if (gio >= 12) {
      lcd.print("(PM)");
    } else {
      lcd.print("(AM)");
    }
  } else {
    lcd.print("      ");
  }
}

void AutoModule() {
  //vào trạng thái auto phải cho biến đếm ở trạng thái manual = 0;
  if (dem != 0) {
    lcd.clear();    //xóa màn hình lcd
    dem = 0;
  }
  DateTime now = rtc.now();
  //Hiển thị giờ
  gio = now.hour();
  phut = now.minute();
  giay = now.second();
  HienThi();
}

void ManualModule() {
  gtmenu = digitalRead(but_format_time); // đọc trạng thái but_format_time và lưu vào biến gtmenu
  if (gtmenu == 0)
  {
    dem += 1;
  }
  delay(200);
  if (dem == 1) { doihienthi(); }
  if (dem == 2) { setgio(); }
  if (dem == 3) { setphut(); }
  if (dem == 4) { setgiay(); }
  if (dem == 5) {
    luusetup();
    delay(500);
  }
}

void doihienthi()
{
  gtcong = digitalRead(ButUp);  // đọc trạng thái ButUp và lưu vào biến gtcong
  gttru = digitalRead(ButDown);// đọc trạng thái ButDown và lưu vào biến gttru
  if (gtcong == 0)      //ktra xem được nhấn không, nếu nhấn thì trạng thái LOW, giá trị nguyên = 0
  {
    type_time == 1 ? type_time = 0 : type_time++;
  }
  if (gttru == 0)
  {
    type_time == 0 ? type_time = 1 : type_time--;
  }
  lcd.setCursor(0, 0);         //đặt vị trí con trỏ lên hàng 0 cột 0
  lcd.print(" CHE DO HIEN THI  ");
  lcd.setCursor(0, 1);
  lcd.print("    THEO: ");
  if (type_time == 0 )
  {
    lcd.setCursor(9, 1);
    lcd.print("24H");
    lcd.setCursor(13, 1);
    lcd.print("     ");
  }
  else if (type_time == 1)
  {
    lcd.setCursor(9, 1);
    lcd.print("12H");
    lcd.setCursor(13, 1);
    lcd.print("   ");
  }
}

void setgio()
{
  gtcong = digitalRead(ButUp);
  gttru = digitalRead(ButDown);
  if (gtcong == 0)
  {
    gio == 23 ? gio = 0 : gio++;
  }
  if (gttru == 0)
  {
    gio == 0 ? gio = 23 : gio--;
  }
  lcd.setCursor(0, 0);
  lcd.print("  CAI DAT GIO   ");
  lcd.setCursor(0, 1);
  lcd.print("    GIO: ");
  if (gio >= 0 &&  gio <= 9)
  {
    lcd.setCursor(9, 1);
    lcd.print(gio);
    lcd.setCursor(10, 1);
    lcd.print("      ");
  }
  else if (gio >= 10 &&  gio <= 24)
  {
    lcd.setCursor(9, 1);
    lcd.print(gio);
    lcd.setCursor(11, 1);
    lcd.print("      ");
  }
  //rtc.setTime(gio, phut, giay);
  delay(100);
}

void setphut()
{
  gtcong = digitalRead(ButUp);
  gttru = digitalRead(ButDown);
  if (gtcong == 0)
  {
    phut == 60 ? phut = 0 : phut++;
  }
  if (gttru == 0)
  {
    phut == 0 ? phut = 60 : phut--;
  }
  lcd.setCursor(0, 0);
  lcd.print("  CAI DAT PHUT  ");
  lcd.setCursor(0, 1);
  lcd.print("    PHUT: ");
  if (phut >= 0 &&  phut <= 9)
  {
    lcd.setCursor(10, 1);
    lcd.print(phut);
    lcd.setCursor(11, 1);
    lcd.print("     ");
  }
  else if (phut >= 10 &&  phut <= 60)
  {
    lcd.setCursor(10, 1);
    lcd.print(phut);
    lcd.setCursor(12, 1);
    lcd.print("     ");
  }

  delay(100);
}

void setgiay()
{
  gtcong = digitalRead(ButUp);
  gttru = digitalRead(ButDown);
  if (gtcong == 0)
  {
    giay == 60 ? giay = 0 : giay++;
  }
  if (gttru == 0)
  {
    giay == 0 ? giay = 60 : giay--;
  }
  lcd.setCursor(0, 0);
  lcd.print("  CAI DAT GIAY  ");
  lcd.setCursor(0, 1);
  lcd.print("    GIAY: ");
  if (giay >= 0 &&  giay <= 9)
  {
    lcd.setCursor(10, 1);
    lcd.print(giay);
    lcd.setCursor(11, 1);
    lcd.print("     ");
  }
  else if (giay >= 10 &&  giay <= 60)
  {
    lcd.setCursor(10, 1);
    lcd.print(giay);
    lcd.setCursor(12, 1);
    lcd.print("     ");
  }
  delay(100);
}

void luusetup()
{
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("SAVE SETTING");
  lcd.setCursor(2, 1);
  lcd.print("PROGRESS....");
  delay(500);
  lcd.clear();
  dem++;
}

void DisplayTemperatureAndHumidity() {
  float temperature = dht.readTemperature(); // Đọc nhiệt độ C từ cảm biến
  float humidity = dht.readHumidity(); // Đọc độ ẩm từ cảm biến
  delay(1000);
  //Hiển thị lên màn hình
  lcd.setCursor(3, 1);
  lcd.print(temperature, 1);//Hiển thị nhiệt độ sau dấu phẩy 1 số
  lcd.setCursor(7, 1);
  lcd.print("C");
  lcd.setCursor(10, 1);
  lcd.print(humidity, 0);//Hiển thị độ ẩm
  lcd.setCursor(12, 1);
  lcd.print("%");

  // Điều chỉnh độ tương phản dựa trên trạng thái của chân sensor
  if (sensorState) {
    // Nếu chân sensor đang nhận tín hiệu 1
    unsigned long currentTime = millis();
    if (currentTime - lastChangeTime >= 300000) { // Kiểm tra nếu đã qua 5 phút
      byte newContrast = 150 + 30; // Tăng độ tương phản
      newContrast = min(255, newContrast); // Đảm bảo không vượt quá giá trị tối đa
      lcd.setContrast(newContrast); // Đặt mức độ tương phản mới
      adjustContrast(newContrast);
      lastChangeTime = currentTime; // Lưu thời điểm cuối cùng điều chỉnh
    }
  } else {
    // Nếu chân sensor đang nhận tín hiệu 0
    unsigned long currentTime = millis();
    if (currentTime - lastChangeTime >= 600000) { // Kiểm tra nếu đã qua 10 phút
      byte newContrast = 150 - 30; // Giảm độ tương phản
      newContrast = max(0, newContrast); // Đảm bảo không nhỏ hơn giá trị tối thiểu
      lcd.setContrast(newContrast); // Đặt mức độ tương phản mới
      adjustContrast(newContrast);
      lastChangeTime = currentTime; // Lưu thời điểm cuối cùng điều chỉnh
    }
  }

}

void adjustContrast(byte contrast) {
  // Gửi lệnh điều chỉnh độ tương phản qua giao tiếp I2C
  Wire.beginTransmission(0x27); // Địa chỉ của màn hình LCD
  Wire.write(0x39); // Lệnh điều chỉnh độ tương phản
  Wire.write(0x70 | (contrast & 0xF)); // Độ tương phản
  Wire.endTransmission();
}

void updateTime() {

  HienThi();
  giay++;
  if (giay == 60) {
    giay = 0;
    phut++;
  }
  if (phut == 60) {
    phut = 0;
    gio++;
  }
  delay(1000);
}

void loop() {
  DisplayTemperatureAndHumidity();
  // put your main code here, to run repeatedly:
  if (type_module == 0) {
    AutoModule();
  }
  else {
    ManualModule();
    if (dem == 6) {
      updateTime();
    }
  }
  int sensorReading = digitalRead(sensor);
  if (sensorReading == 1 && !sensorState) {
    // Nếu nhận được tín hiệu 1 từ chân sensor và trạng thái trước đó là 0
    sensorState = true; // Cập nhật trạng thái của chân sensor
    lastChangeTime = millis(); // Lưu thời điểm của sự thay đổi
  } else if (sensorReading == 0 && sensorState) {
    // Nếu nhận được tín hiệu 0 từ chân sensor và trạng thái trước đó là 1
    sensorState = false; // Cập nhật trạng thái của chân sensor
  }
}
