/*
This program uses an arduino to collect flight mission parameters
from a user. It then calculates and performs the correct timing of
shutter events given the input.
 */

#include <Arduino.h>
#include <LiquidCrystal.h>
#include <math.h>

enum Button { up, down, right, left, select, none, unknown };
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);
const int PWM_PIN = 3;
const float TRIGGER_TIME_MS = 100;
const float FORWARD_OVERLAP = 70;
float time_ms;

Button readinput() {
  int adc_key_in = analogRead(0);
  Button pressed = unknown;
  if (adc_key_in < 50) pressed = right;
  if ((adc_key_in > 50) && (adc_key_in < 250)) pressed = up;
  if ((adc_key_in > 250) && (adc_key_in < 400)) pressed = down;
  if ((adc_key_in > 400) && (adc_key_in < 650)) pressed = left;
  if ((adc_key_in > 650) && (adc_key_in < 800)) pressed = select;
  if ((adc_key_in > 800) && (adc_key_in < 1150)) pressed = none;
  return pressed;
}

void setup() {
  float height_ft = 400;
  float speed_mph = 15;
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("starting up");
  delay(2000);

  // get flight height and speed
  Button status = none;
  while (status != select) {
    lcd.clear();
    status = readinput();
    lcd.setCursor(0, 0);
    lcd.print("height    speed");
    lcd.setCursor(0, 1);
    lcd.print(height_ft);
    lcd.print("    ");
    lcd.print(speed_mph);
    if (status == up) height_ft += 10;
    if (status == down) height_ft -= 10;
    if (status == right) speed_mph += 1;
    if (status == left) speed_mph -= 1;
    delay(120);
  }

  // calculate trigger time
  delay(1000);
  // button pressed - save settings
  // pwm_pin = 6;
  float height = height_ft * 0.3048;  // m
  float speed = speed_mph * 0.44704;  // m/s
  // float s_width = 000;          // mm
  float s_focal = 55;  // mm
  // float img_width = 8280;       // pix
  float img_height = 6208;   // pix
  float pix_pitch = 0.0053;  // mm

  float gsd = pix_pitch * height / s_focal;                 // m
  float length = img_height * gsd;                          // m
  float distance = length * (100 - FORWARD_OVERLAP) / 100;  // m
  float time = distance / speed;                            // s

  // confirm time interval, or manual change
  status = none;
  bool change_time = false;
  while (status != select) {
    status = readinput();
    if (status == left) change_time = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    if (!change_time) {
      lcd.print("time  [s] | [<-]");
    } else {
      Button ch_status = none;
      while (ch_status != select) {
        ch_status = readinput();
        lcd.clear();
        lcd.print("time [^ | v]");
        lcd.setCursor(0, 1);
        lcd.print(time, 1);
        if (ch_status == up) time += 0.1;
        if (ch_status == down) time -= 0.1;
        delay(120);
      }
      change_time = false;
      status = none;
      lcd.clear();
    }
    lcd.setCursor(0, 1);
    lcd.print(time, 1);
  }
  delay(1000);

  // convert time to milliseconds
  time_ms = time * 1000;

  // get time to wait before triggering
  lcd.clear();
  status = none;
  int time_to_wait = 45;
  while (status != select) {
    status = readinput();
    lcd.setCursor(0, 0);
    lcd.print("delay [^ | v]");
    lcd.setCursor(0, 1);
    lcd.print(time_to_wait);
    if (status == up) time_to_wait += 10;
    if (status == down) time_to_wait -= 10;
    delay(120);
  }
  delay(1000);

  // confirm mission start
  lcd.clear();
  status = none;
  lcd.setCursor(0, 0);
  lcd.print("Press select to");
  lcd.setCursor(0, 1);
  lcd.print("start mission");
  while (status != select) {
    status = readinput();
  }
  delay(1000);

  // start message
  lcd.setCursor(0, 0);
  lcd.print("triggering starts ");
  lcd.setCursor(0, 1);
  lcd.print("in ");
  lcd.print(time_to_wait);
  lcd.print(" seconds");
  delay(time_to_wait * 1000.0);
  lcd.noDisplay();
}

void loop() {
  // wait calculated seconds
  analogWrite(PWM_PIN, 255);
  delay(time_ms);
  // trigger
  analogWrite(PWM_PIN, 0);
  delay(TRIGGER_TIME_MS);
}
