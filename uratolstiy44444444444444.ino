#define SMOOTH_ALGORITHM
#define CLK 3
#define DT 4
#define SW 2
#define DIR 7
#define EN 9
#define STEP 8
#define BUTTON_PIN_2 12
#define BUTTON_PIN_1 13
#define BUTTON_PIN_3 11
#define BUTTON_PIN_4 10

#include <GyverStepper2.h>
#include <GyverStepper.h>
#include <LiquidCrystal_I2C.h>
#include <GyverEncoder.h>
Encoder enc(CLK, DT, SW);
GStepper2<STEPPER2WIRE> stepper2(3200, STEP, DIR, EN);
GStepper<STEPPER2WIRE> stepper(3200, STEP, DIR, EN);
LiquidCrystal_I2C lcd(0x27, 20, 4);
bool first_button = 0;
bool second_button = 0;
bool third_button = 0;
unsigned long angle_for_2_regim = 0;
int speed_pov_for_2_regim = 0;
bool angle_speed_select_for_2_reg = 0;
bool mode_for_2_reg = 0;
int speed_pov = 0;
int regim = 0;
unsigned long tmr;
bool btn_state;
bool prev_btn = 0;
bool prev_btn_for_second = 0;
bool mode_normal = 1;
int nul = 0;
int count = 0;
void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN_1, INPUT);
  pinMode(BUTTON_PIN_2, INPUT);
  pinMode(BUTTON_PIN_3, INPUT);
  enc.setType(TYPE2);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Mode-idle. If you");
  lcd.setCursor(0, 1);
  lcd.print("want to change the");
  lcd.setCursor(0, 2);
  lcd.print("mode, hold and");
  lcd.setCursor(0, 3);
  lcd.print("scroll the encoder.");
  stepper.enable();
  stepper.setRunMode(KEEP_SPEED);
  stepper.setSpeed(speed_pov, true);
  stepper.setAcceleration(1000);
  stepper.autoPower(1);
  stepper2.enable();
  stepper2.setMaxSpeed(speed_pov_for_2_regim);
  stepper2.setAcceleration(10000);
  stepper2.autoPower(1);
}

void loop() {
  first_button = digitalRead(BUTTON_PIN_1);
  second_button = digitalRead(BUTTON_PIN_2);
  third_button = digitalRead(BUTTON_PIN_3);
  enc.tick();
  button();

  if (mode_normal == 1) {
    if (enc.isRightH()) {
      regim += 1;
      if (regim > 2) regim = 0;
      if (regim == 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Mode-idle. If you");
        lcd.setCursor(0, 1);
        lcd.print("want to change the");
        lcd.setCursor(0, 2);
        lcd.print("mode, hold and");
        lcd.setCursor(0, 3);
        lcd.print("scroll the encoder.");
      } else if (regim == 1) {
        stepper2.disable();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Regim number one: ON");
      } else if (regim == 2) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Regim number two:ON");
      }
    }
    if (enc.isLeftH()) {
      regim -= 1;
      if (regim < 0) regim = 2;
      if (regim == 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Mode-idle. If you");
        lcd.setCursor(0, 1);
        lcd.print("want to change the");
        lcd.setCursor(0, 2);
        lcd.print("mode, hold and");
        lcd.setCursor(0, 3);
        lcd.print("scroll the encoder.");
      } else if (regim == 1) {
        stepper2.disable();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Regim number one: ON");

      } else if (regim == 2) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Regim number two:ON");
      }
    }

    if (regim == 1) {
      regim_1();
    } else if (regim == 2) {
      regim_2();
    }
  } else{
    lcd.setCursor(19, 3);
    lcd.print(" ");
    stepper.disable();
  }
}
void regim_1() {
  stepper.tick();
  if (enc.isRight()) {
    speed_pov += 50;
    if (speed_pov > 30000) speed_pov = 30000;
    if (speed_pov == 0) {
      stepper2.disable();
      lcd.clear();
      lcd.print("Speed:");
      lcd.setCursor(7, 0);
      lcd.print(speed_pov);
    } else {
      stepper.setSpeed(speed_pov, true);
      lcd.clear();
      lcd.print("Speed:");
      lcd.setCursor(7, 0);
      lcd.print(speed_pov);
    }
  }
  if (enc.isLeft()) {
    speed_pov -= 50;
    if (speed_pov < 0) speed_pov = 0;
    if (speed_pov == 0) {
      stepper2.disable();
      lcd.clear();
      lcd.print("Speed:");
      lcd.setCursor(7, 0);
      lcd.print(speed_pov);
    } else {
      stepper.setSpeed(speed_pov, true);
      lcd.clear();
      lcd.print("Speed:");
      lcd.setCursor(7, 0);
      lcd.print(speed_pov);
    }
  }

  if (enc.isSingle()) {
    speed_pov = 0;
    stepper2.disable();
    lcd.clear();
    lcd.print("Speed:");
    lcd.setCursor(7, 0);
    lcd.print(speed_pov);
  }
  if (first_button == 1 and second_button == 0) {
    digitalWrite(DIR, LOW);
  }
  if (second_button == 1 and first_button == 0) {
    digitalWrite(DIR, HIGH);
  }
}

void regim_2() {
  stepper2.enable();
  stepper2.autoPower(0);
  stepper2.tick();

  if (mode_for_2_reg == 0) {
    switch (angle_speed_select_for_2_reg) {
      case 0:
        if (enc.isRight()) {
          angle_for_2_regim += 50;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Angle:");
          lcd.setCursor(7, 0);
          lcd.print(angle_for_2_regim);
        }
        if (enc.isLeft()) {
          angle_for_2_regim -= 50;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Angle:");
          lcd.setCursor(7, 0);
          lcd.print(angle_for_2_regim);
        }
        break;


      case 1:
        if (enc.isRight()) {
          speed_pov_for_2_regim += 50;
          if (speed_pov_for_2_regim >= 30000) speed_pov_for_2_regim = 30000;
          if (speed_pov_for_2_regim == 0) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Speed:");
            lcd.setCursor(7, 0);
            lcd.print(speed_pov_for_2_regim);
          } else {
            stepper2.setMaxSpeed(speed_pov_for_2_regim);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Speed:");
            lcd.setCursor(7, 0);
            lcd.print(speed_pov_for_2_regim);
          }
        }
        if (enc.isLeft()) {
          speed_pov_for_2_regim -= 50;
          if (speed_pov_for_2_regim <= 0) speed_pov_for_2_regim = 0;
          if (speed_pov_for_2_regim == 0) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Speed:");
            lcd.setCursor(7, 0);
            lcd.print(speed_pov_for_2_regim);
          } else {
            stepper2.setMaxSpeed(speed_pov_for_2_regim);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Speed:");
            lcd.setCursor(7, 0);
            lcd.print(speed_pov_for_2_regim);
          }
        }
        break;
    }
  } else {

    if (third_button == 1 and stepper2.getCurrent() == nul and prev_btn_for_second == 0) {

      count += 1;
      stepper2.setTarget(angle_for_2_regim, ABSOLUTE);
    }
    if (stepper2.getCurrent() == angle_for_2_regim) {
      stepper2.setTarget(-angle_for_2_regim, ABSOLUTE);
    }
    if (stepper2.getCurrent() == -angle_for_2_regim) {
      stepper2.setTarget(nul, ABSOLUTE);
    }
    prev_btn_for_second = third_button;
  }
  if (enc.isSingle()) {
    mode_for_2_reg = !mode_for_2_reg;
    if (mode_for_2_reg == 1) {
      lcd.clear();
      lcd.print("press btn");
    }
    if (mode_for_2_reg == 0) {
      lcd.clear();
      lcd.print("select ang and spd");
    }
  }
  if (first_button == 1 and second_button == 0) {
    angle_speed_select_for_2_reg = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Angle:");
    lcd.setCursor(7, 0);
    lcd.print(angle_for_2_regim);
  }
  if (second_button == 1 and first_button == 0) {
    angle_speed_select_for_2_reg = 1;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Speed:");
    lcd.setCursor(7, 0);
    lcd.print(speed_pov_for_2_regim);
  }
}
void button() {
  btn_state = digitalRead(BUTTON_PIN_4);
  if (btn_state == 1 and prev_btn == 0) {
    mode_normal = !mode_normal;
  }
  prev_btn = btn_state;
}