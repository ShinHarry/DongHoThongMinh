#define sensor 10
#define led 7

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "RTClib.h"

RTC_DS1307 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

volatile int but_set_module = 2, type_module = 0;  //0:auto(default) 1:manual
volatile int but_format_time = 3, type_time = 0; // 0:24h(default) 1:12h am-pm
int ButUp = 1;
int ButDown = 0;
void setup() {
  Serial.begin(9600);
  lcd.init(); // thiết lập các cài đặt cho màn hình LCD
  lcd.backlight(); // bật đèn nền
  if(!rtc.begin()){
    lcd.print("Couldn't find rtc");
    while(1);
  }

  if(!rtc.isrunning()){
    lcd.print("RTC is not running!");
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); //điều chỉnh thời gian của RTC khớp với thời gian hiện tại của hệ thống
  // button transform module auto/manual
  pinMode(but_set_module, INPUT);
  attachInterrupt(0, trans_module, FALLING);
  // button transform type time display
  pinMode(but_format_time, INPUT);
  attachInterrupt(1, trans_type, FALLING);
  // Button Up-Down
  pinMode(ButUp, INPUT);
  pinMode(ButDown, INPUT);
  //
  pinMode(sensor, INPUT);
  pinMode(led, OUTPUT);
}
void trans_module(){
  type_module = !type_module; // Chuyển đổi giữa auto và manual
}
void trans_type(){
  type_time = !type_time; // Chuyển đổi giữa 24h và 12h
}
void AutoModule(){
  DateTime now = rtc.now();
  //Hiển thị giờ
  int hour = now.hour();
  if (type_time == 1) {
    // 12h AM/PM
    lcd.setCursor(0, 0);
    lcd.print("  ");
    if (hour == 0 || hour == 12) {
      lcd.print("12");
    } else {
      lcd.print(hour % 12);
    }
  } else {
    // 24h
    lcd.setCursor(2, 0);
    lcd.print("  ");
    if (now.hour() <= 9) {
      lcd.print("0");
      lcd.print(now.hour());
    } else {
      lcd.print(now.hour());
    }
  }
  //Hiển thị phút
  lcd.print(":");

  if (now.minute() <= 9) {
    lcd.print("0");
    lcd.print(now.minute());
  } else {
    lcd.print(now.minute());
  }

  lcd.print(":");
  //Hiển thị giây
  if (now.second() <= 9) {
    lcd.print("0");
    lcd.print(now.second());
  } else {
    lcd.print(now.second());
  }
  //Hiển thị hậu tố
  if (type_time == 1){
    if (hour >= 12) {
      lcd.print("(PM)");
    } else {
      lcd.print("(AM)");
    }
  } else {
    lcd.print("      ");
  }

}

void ManualModule(){
  //code here
}
void loop() {
  // put your main code here, to run repeatedly:
  if(type_module == 0){
    AutoModule();
  }
  else{
    ManualModule();
  }
  digitalWrite(led, digitalRead(sensor));
  Serial.println(digitalRead(sensor));
}
