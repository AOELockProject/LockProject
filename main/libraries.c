#include <Servo.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <pitches.h>
#include <SPI.h>
#include <MFRC522.h>

void RGB_color(int red, int green, int blue)
{
  analogWrite(RGB_pins[0], red);
  analogWrite(RGB_pins[1], green);
  analogWrite(RGB_pins[2], blue);
}

void RGB_color_blue()
{
  RGB_color(0, 0, 255);
}

void RGB_color_red()
{
  RGB_color(255, 0, 0);
}

void RGB_color_green()
{
  RGB_color(0, 255, 0);
}

void LCDPasswordDisplaySet(int line, int offset)
{
  lcd.setCursor(line, offset);
  lcd.print('*');
}

// setup 初始化
/* LCD, servo, RGB, keypad, Buzzer */

void LCD_Init()
{
  lcd.begin(16, 2);
}

void servo_Init(const int servoPin)
{
  myservo.attach(servoPin);
}

void RGB_Init()
{
  for (int i = 0; i < sizeof(RGB_pins) / sizeof(const int); i++)
  {
    pinMode(RGB_pins[i], OUTPUT);
  }
  RGB_color(0, 0, 0);
}

void keypad_Init()
{
  keypad.addEventListener(keypadEvent); // Add an event listener for this keypad
}

void Buzzer_Init(int BuzzerPin, int noteDuration)
{
  pinMode(BuzzerPin, OUTPUT);
  noteDuration = 4;
}

void RFID_Init()
{
  SPI.begin();
  rfid.PCD_Init();
}

void printHex(byte *buffer, byte bufferSize)
{
  for (byte i = 0; i < bufferSize; i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : "");
    Serial.print(buffer[i], HEX);
  }
}

void printDec(byte *buffer, byte bufferSize)
{
  for (byte i = 0; i < bufferSize; i++)
  {
    Serial.print(buffer[i] < 0x10 ? " 0" : "");
    Serial.print(buffer[i], DEC);
  }
}

//键盘单个字符读入
char keyRead()
{
  return keypad.getKey();
}

void keypadEvent(KeypadEvent key)
{
  switch (keypad.getState())
  {
  case PRESSED:
    Serial.print(key);
    if (key == '#')
    {
      // clear
      Serial.print('#');
    }
    if (key == '*')
    {
      // confirm
    }
    break;
  case RELEASED:
    if (key == '*')
    {
    }
    break;
  case HOLD:
    if (key == '*')
    {
    }
    break;
  }
}

//舵机转向  servo rotate function
/* 接收到开门/关门信号,舵机旋转
  1.刷卡
  2.指纹
  3.键盘密码输入
*/
void servo_turn()
{
  if (doorStatus == 1)
  {
    for (pos; pos <= 180; pos++)
    {
      myservo.write(pos);
      delay(1);
    }
    doorStatus = CLOSED;
  }
  else
  {
    for (pos; pos >= 0; pos--)
    {
      myservo.write(pos);
      delay(1);
    }
    doorStatus = OPENED;
  }
}

//蜂鸣器模块
void buzzerSpeaker(long noteDuration, int BuzzerPin, boolean num)
{
  int melody[]{
      NOTE_G4, NOTE_A4, 0};
  int noteLength = 1000 / noteDuration;
  if (num)
  {
    for (int i = 0; i < sizeof(melody) / sizeof(int); i++)
    {
      tone(BuzzerPin, melody[i], noteLength);
      delay(noteLength);
      noTone(BuzzerPin);
    }
  }
  else
  {
    tone(BuzzerPin, melody[0], noteLength);
    delay(noteLength);
    noTone(BuzzerPin);
  }
}

// LCD显示模块 display module
/*
  默认显示Enter the Password:
  1.键盘输入时显示密码输入界面***
*/

void defaultDisplay()
{
  lcd.setCursor(0, 0);
  lcd.print("Password:");
}

//密码错误LCD显示
void LCDPasswordWrong()
{
  lcd.setCursor(0, 0);
  lcd.print("Wrong PW!");
}

// 正确密码显示
void LCDPasswordRight()
{
  lcd.setCursor(0, 0);
  lcd.print("PASS!");
}

//键盘输入字符与预设密码比较
boolean passwordCompare(char inputkey, char *password, int place)
{
  char *pass_p = password + place;
  if (*pass_p != inputkey)
  {
    return false;
  }
  return true;
}

// 密码错误警报
void passwordWrong()
{
  LCDPasswordWrong();
  RGB_color_red();
  buzzerSpeaker(noteDuration, BuzzerPin, 0);
  servo_turn();
  delay(5000);
  defaultDisplay();
  RGB_Init();
  servo_turn();
}

// 密码正确
void passwordRight()
{
  LCDPasswordRight();
  RGB_color_green();
  buzzerSpeaker(noteDuration, BuzzerPin, 1);
  servo_turn();
  delay(5000);
  servo_turn();
  RGB_Inti();
  defaultDisplay();
}

//字符串长度计算
int _strlen(char *str)
{
  char *ptr = str;
  int len = 0;
  while (*ptr != NULL)
  {
    len++;
  }
  return len;
}

//输入密码
/* length 表示密码长度 */
void enterPassword(int length)
{
  int i = 0;
  while (length--)
  {
    char key = keyRead();
    In_buffer[i] = key;
    i++;
  }
}

//输入成功后密码比较
boolean inputPasswordCompared(char *input, char *password)
{
  boolean result = false;
  if (!_strlen(input) == _strlen(password))
  {
    return result;
  }
  else
  {
    while (*input != NULL && *password != NULL)
    {
      if (*input == NULL || *password == NULL || *input != *password)
      {
        result = false;
        return result;
      }
      result = true;
      input++;
      password++;
    }
    return result;
  }
}

//输入模式选择 
int modeSelect() {
  return keypad.getKey() - '0';
}