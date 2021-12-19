/* UNO Serial & LCD setting */
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(1, 0);     // uno RX, TX

// LCD 初始化
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void lcd_Init() {
    lcd.begin(16, 2);
}

// LCD 功能函数
void lcd_WelcomeDisplay() {
    /* Default */
    lcd.setCursor(0, 0);
    lcd.print("WELCOME!");
}

void lcd_InputPasswordDisplay() {
    /* i */
    lcd.setCursor(0, 0);
    lcd.print("PASSWORD:");
}

void lcd_WrongDisplay() {
    /* w */
    lcd.setCursor(0, 0);
    lcd.print("WRONG!");
}

void lcd_PassgDisplay() {
    /* p */
    lcd.setCursor(0, 0);
    lcd.print("PASS!");
}

void lcd_RFIDErrorDisplay() {
    /* c */
    lcd.setCursor(0, 0);
    lcd.print("CARD ERROR!");
}

void setup() {
    lcd_Init();
    mySerial.begin(9600);
}

void loop() {
    lcd_WelcomeDisplay();
    char input = mySerial.read();
    switch (input) {
        case 'p': {
            lcd_PassgDisplay();
            break;
        }
        case 'w': {
            lcd_WrongDisplay();
            break;
        }
        case 'i': {
            lcd_InputPasswordDisplay();
            break;
        }
        case 'c': {
            lcd_RFIDErrorDisplay();
            break;
        }
        default : {}
    }
    delay(1000);
}