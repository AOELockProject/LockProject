#include <Servo.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <pitches.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

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
  defaultDisplay();
  RGB_Init();
}

// 密码正确
void passwordRight()
{
  LCDPasswordRight();
  RGB_color_green();
  buzzerSpeaker(noteDuration, BuzzerPin, 1);
  openLock();
  delay(5000);
  closeLock();
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
void enterPassword(void)
{
  int i = 0;
  char key;
  while ((key = keyRead()) != '*');
  {
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

// 输入模式选择 
int modeSelect() {
  return keypad.getKey() - '0';
}

// 开锁落锁
void openLock(){
    servo_turn();
}
void closeLock() {
    servo_turn();
}

// 指纹识别初始化
void fingerPrint_Init()
{
    finger.begin(57600);
    if (finger.verifyPassword())
    {
        Serial.println("Found fingerprint sensor!");
    }
    else
    {
        Serial.println("Did not find fingerprint sensor :(");
        return;
    }

    delay(1);
}

// 指纹ID 选择
uint8_t readnumber(void)
{
    uint8_t num = 0;

    while (num == 0)
    {
        num = keyRead() - '0';
    }
    return num;
}

// Enroll fingerprint
uint8_t getFingerprintEnroll()
{

    int p = -1;
    Serial.print("Waiting for valid finger to enroll as #");
    Serial.println(id);
    while (p != FINGERPRINT_OK)
    {
        p = finger.getImage();
        switch (p)
        {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.println(".");
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            break;
        default:
            Serial.println("Unknown error");
            break;
        }
    }

    // OK success!

    p = finger.image2Tz(1);
    switch (p)
    {
    case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
    default:
        Serial.println("Unknown error");
        return p;
    }

    Serial.println("Remove finger");
    delay(2000);
    p = 0;
    while (p != FINGERPRINT_NOFINGER)
    {
        p = finger.getImage();
    }
    Serial.print("ID ");
    Serial.println(id);
    p = -1;
    Serial.println("Place same finger again");
    while (p != FINGERPRINT_OK)
    {
        p = finger.getImage();
        switch (p)
        {
        case FINGERPRINT_OK:
            Serial.println("Image taken");
            break;
        case FINGERPRINT_NOFINGER:
            Serial.print(".");
            break;
        case FINGERPRINT_PACKETRECIEVEERR:
            Serial.println("Communication error");
            break;
        case FINGERPRINT_IMAGEFAIL:
            Serial.println("Imaging error");
            break;
        default:
            Serial.println("Unknown error");
            break;
        }
    }

    // OK success!

    p = finger.image2Tz(2);
    switch (p)
    {
    case FINGERPRINT_OK:
        Serial.println("Image converted");
        break;
    case FINGERPRINT_IMAGEMESS:
        Serial.println("Image too messy");
        return p;
    case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        return p;
    case FINGERPRINT_FEATUREFAIL:
        Serial.println("Could not find fingerprint features");
        return p;
    case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Could not find fingerprint features");
        return p;
    default:
        Serial.println("Unknown error");
        return p;
    }

    // OK converted!
    Serial.print("Creating model for #");
    Serial.println(id);

    p = finger.createModel();
    if (p == FINGERPRINT_OK)
    {
        Serial.println("Prints matched!");
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Communication error");
        return p;
    }
    else if (p == FINGERPRINT_ENROLLMISMATCH)
    {
        Serial.println("Fingerprints did not match");
        return p;
    }
    else
    {
        Serial.println("Unknown error");
        return p;
    }

    Serial.print("ID ");
    Serial.println(id);
    p = finger.storeModel(id);
    if (p == FINGERPRINT_OK)
    {
        Serial.println("Stored!");
        digitalWrite(LED_pin[1], HIGH);
        delay(2000);
        digitalWrite(LED_pin[1], LOW);
    }
    else if (p == FINGERPRINT_PACKETRECIEVEERR)
    {
        Serial.println("Communication error");
        return p;
    }
    else if (p == FINGERPRINT_BADLOCATION)
    {
        Serial.println("Could not store in that location");
        return p;
    }
    else if (p == FINGERPRINT_FLASHERR)
    {
        Serial.println("Error writing to flash");
        return p;
    }
    else
    {
        Serial.println("Unknown error");
        return p;
    }

    return true;
}

// finger Detect function
int getFingerprintIDez()
{
    uint8_t p = finger.getImage();
    while ((p = finger.getImage()) != FINGERPRINT_OK);

    p = finger.image2Tz();
    if (p != FINGERPRINT_OK)
        return -1;

    p = finger.fingerFastSearch();
    if (p != FINGERPRINT_OK)
        return -1;

    if (finger.fingerID == 1)
    {
        digitalWrite(LED_pin[1], HIGH);
        delay(2000);
        digitalWrite(LED_pin[1], LOW);
    }

    // found a match!
    return finger.fingerID;
}