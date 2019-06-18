#include <Arduino.h>
#include <LiquidCrystal.h>

int pwm_pin = 6;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
int pot_time = 2;
float time = 2.0;
float trigger_time_ms = 200;

void setup() {
  lcd.begin(16, 2);
  lcd.print("Starting");
  delay(2000);
}

void loop() {
  lcd.clear();
  lcd.setCursor(0, 0);
  time = analogRead(pot_time) / 200.0 + 1.5;
  lcd.print("time: ");
  lcd.print(time);

  analogWrite(pwm_pin, 255);
  delay(time * 1000);
  // trigger
  analogWrite(pwm_pin, 0);
  delay(trigger_time_ms);
}