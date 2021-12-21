#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(1, 0);

// LCD 初始化
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void lcd_Init() {
    lcd.begin(16, 2);
}

// LCD 功能函数
void lcd_WelcomeDisplay() {
    /* Default */
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WELCOME!");
}

void lcd_InputPasswordDisplay() {
    /* i */
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PASSWORD:");
}

void lcd_WrongDisplay() {
    /* w */
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WRONG!");
}

void lcd_PassgDisplay() {
    /* p */
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("PASS!");
}

void lcd_RFIDErrorDisplay() {
    /* c */
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("CARD ERROR!");
}

void setup() {
    lcd_Init();
    Serial.begin(19200);
}

int i = 0;
void loop() {
  lcd_WelcomeDisplay();

  char input;

  while (true) {
    if (mySerial.available()) {
      input = mySerial.read();
      break;
    }
  }

  switch (input) {
    // input password from Keypad
    case 'i': {
      lcd_InputPasswordDisplay();
      char choose;
      while (true) {
        if (mySerial.available()) {
          choose = mySerial.read();
          break;
        }
      }

      if (choose == 'w') {
        lcd_WrongDisplay();
      } else if (choose == 'p') {
        lcd_PassgDisplay();
      }

      break;
    }


    // RFID Card 
    case 'c': {
      char choose;
      while (true) {
        if (mySerial.available()) {
          choose = mySerial.read();
          break;
        }
      }

      if (choose == 'w') {
        lcd_WrongDisplay();
      } else if (choose == 'p') {
        lcd_PassgDisplay();
      }

      break;
    }

    // fingerprint
    case 'f': {
      char choose;
      while (true) {
        if (mySerial.available()) {
          choose = mySerial.read();
          break;
        }
      }

      if (choose == 'w') {
        lcd_WrongDisplay();
      } else if (choose == 'p') {
        lcd_PassgDisplay();
      }

      break;
    }
    default : {}
  }
  delay(3000);
}
