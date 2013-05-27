#include <Arduino.h>

#include "nancylib.h"

#define I2C_ADD    0xC0

#define LED_PIN    8
#define UP_PIN     10
#define DOWN_PIN   9

#define EN_PIN     7
#define MS1_PIN    6
#define MS2_PIN    5
#define RST_PIN    3
#define SLP_PIN    2
#define STEP_PIN   1
#define DIR_PIN    0


void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  pinMode(UP_PIN, INPUT);
  digitalWrite(UP_PIN, HIGH);
  pinMode(DOWN_PIN, INPUT);
  digitalWrite(DOWN_PIN, HIGH);
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, HIGH);
  pinMode(MS1_PIN, OUTPUT);
  digitalWrite(MS1_PIN, LOW);
  pinMode(MS2_PIN, OUTPUT);
  digitalWrite(MS2_PIN, LOW);
  pinMode(RST_PIN, OUTPUT);
  digitalWrite(RST_PIN, HIGH);
  pinMode(SLP_PIN, OUTPUT);
  digitalWrite(SLP_PIN, HIGH);
  pinMode(STEP_PIN, OUTPUT);
  digitalWrite(STEP_PIN, HIGH);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(DIR_PIN, HIGH);
}

int current = -1;
int time = millis();
int longs = 0;
int shorts = 0;
boolean up_state;
boolean down_state;
int up_cnt;
int down_cnt;
void loop() {
  if(time > -1) {
  // check buttons
  boolean up_btn = digitalRead(UP_PIN);
  boolean down_btn = digitalRead(DOWN_PIN);
  if(up_btn == up_state) {
    up_cnt = 0;
  }
  else {
    up_cnt++;
    if(up_cnt > 50) {
      up_state = up_btn;
      up_cnt = 0;
      if(up_state) up_pushed();
    }
  }
  if(down_btn == down_state) {
    down_cnt = 0;
  }
  else {
    down_cnt++;
    if(down_cnt > 50) {
      down_state = down_btn;
      down_cnt = 0;
      if(down_state) down_pushed();
    }
  }
  }
  
  // read current setting
  int new_current;
  if(connection_test(STEP_PIN, DIR_PIN, I2C_ADD)) {
    new_current = driver_read(STEP_PIN, DIR_PIN, I2C_ADD);
  }
  else {
    new_current = -1;
  }
  if(new_current == -1){
    time = -1;
  }
  
  if(new_current != current && new_current > -1) {
    current = new_current;
    longs = 0;
    shorts = 0;
    int t = current;
    while(t >= 100) {
      t -= 100;
      longs++;
    }
    while(t >= 10) {
      t -= 10;
      shorts++;
    }
    time = millis();
  }

  // flash current setting
  if(time > -1) {
    flash_current();
  }
}

void up_pushed() {
  int data = current + 10;
  if(data > 250) data = 250;
  driver_write(STEP_PIN, DIR_PIN, I2C_ADD, data);
}

void down_pushed() {
  int data = current - 10;
  if(data < 0) data = 0;
  driver_write(STEP_PIN, DIR_PIN, I2C_ADD, data);
}

void flash_current() {
  int state = millis() - time;
  for (int i=0;i<longs;i++) {
    if(time < (2*i+1)*1000) {
      digitalWrite(LED_PIN, HIGH);
      return;
    }
    else if(time < (2*i+2)*1000) {
      digitalWrite(LED_PIN, LOW);
      return;
    }
  }
  int basetime = 2*longs*1000;
  for (int i=0;i<shorts;i++) {
    if(time < basetime+(2*i+1)*500) {
      digitalWrite(LED_PIN, HIGH);
      return;
    }
    else if(time < basetime+(2*i+2)*500) {
      digitalWrite(LED_PIN, LOW);
      return;
    }
  }
  basetime += 2*shorts*500 + 1000;
  if(time > basetime) time = millis();
}
