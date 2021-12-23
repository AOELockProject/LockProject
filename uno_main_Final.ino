#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(1, 0);

// LCD 初始化
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int clock = 1;

void lcd_Init() {
    lcd.begin(16, 2);
}

// LCD 功能函数
void lcd_WelcomeDisplay() {
    /* Default */
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("0:Card 1:Password");
    lcd.setCursor(0, 1);
    lcd.print("2:Fingerprint");
}

void lcd_InputPasswordDisplay() {
    /* i */
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Password:");
}

void lcd_WrongDisplay() {
    /* w */
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wrong!");
}

void lcd_PassgDisplay() {
    /* p */
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Pass!");
}



void setup() {
    lcd_Init();
    mySerial.begin(19200);
    //Serial.begin(4800);
}

int i = 0;
void loop() {
  
  //mySerial.println("可以打印");
  char input;
  while (clock) {
    lcd_WelcomeDisplay();
    clock = 0;
    }
  

  while (true) {
    if (mySerial.available()) {
      input = mySerial.read();
      break;
    }
  }

  switch (input) {
    // input password from Keypad
    case 'i': {
      //mySerial.println("shu ka mo shi");
      lcd_InputPasswordDisplay();

      //delay(3000);
      char choose;
      while (true) {
        if (mySerial.available()) {
          
              choose = mySerial.read();
              break;
            }else{
              
            }
          
        }
      
           
      if (choose == 'w') {
        lcd_WrongDisplay();
        clock = 1;
        delay(1000);
      } else if (choose == 'p') {
        lcd_PassgDisplay();
        clock = 1;
        delay(5000);
      }

      break;
    }


    // RFID Card 
    case 'c': {
      char choose;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Please swipe");
      lcd.setCursor(0, 1);
      lcd.print("card!");
      
      while (true) {
        if (mySerial.available()) {
          choose = mySerial.read();
          break;
        }else{
          
        }

      }

      if (choose == 'w') {
        lcd_WrongDisplay();
        clock = 1;
        delay(1000);
      } else if (choose == 'p') {
        lcd_PassgDisplay();
        clock = 1;
        delay(5000);
      }

      break;
    }

    // fingerprint
    case 'f': {
      char choose;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("3:Press 4:Enrol");
      lcd.setCursor(0, 1);
      lcd.print("5:Delete");
      while (true) {
        if (mySerial.available()) {
          choose = mySerial.read();
          break;
        }else{
  
        }
      }
      if(choose == 'r'){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Please press fingerprint!");
        lcd.setCursor(0, 1);
        lcd.print("fingerprint!");
      }else if(choose == 'e' || choose == 'd'){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Password:");
      }
      
      while (true) {
        if (mySerial.available()) {
          choose = mySerial.read();
          break;
        }else{
  
        }
      }
     
      if (choose == 'w') {
        lcd_WrongDisplay();
        clock = 1;
        delay(1000);
        break;
      } else if (choose == 'p') {
        lcd_PassgDisplay();
        clock = 1;
        delay(5000);
        break;
      }else if (choose == 'l'){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Please press new");
        lcd.setCursor(0, 1);
        lcd.print("fingerprint!");

        while (true) {
          if (mySerial.available()) {
            choose = mySerial.read();
            break;
          }
        }

          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Fingerprint successfully");
          lcd.setCursor(0, 1);
          lcd.print("enroled!");
          delay(1000);
        clock = 1;

        break;
      }
      else if (choose == 'x'){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Fingerprint successfully");
        lcd.setCursor(0, 1);
        lcd.print("deleted!");
        delay(1000);
        clock = 1;
        break;
      }
      


      break;
    }
    default : {}
  }
  delay(20);
}
