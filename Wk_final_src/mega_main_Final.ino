#include <usrlib.h>

SoftwareSerial fingerSerial(18, 19); // 18: mega TX -> s608 RX / 19:mega RX -> s608 TX
SoftwareSerial unoSerial(16, 17);    // 14: mega TX / 15: RX1
#define fingerSerial Serial1
#define unoSerial Serial2
// fingerprint
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&fingerSerial);
uint8_t id;
int LED_pins[] = {35, 36, 37};     // yellow:RFID run, green:Keypad run, red: fingerprint

int PassMode = sRFID;

// keypad listener
void keypadEvent(KeypadEvent key);

// Keypad
const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

byte rowPins[ROWS] = {46, 41, 42, 44};
byte colPins[COLS] = {45, 47, 43};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// keypad.addEventListener(keypadEvent); // Add an event listener for this keypad

// Servo
Servo myservo;
const int servoPin = 26;
int pos = 0;

// Speaker
int noteDuration = 4, BuzzerPin = 25;

// RGB
const int RedPin = 22, GreenPin = 23, BluePin = 24;
const int RGB_pins[3] = {RedPin, GreenPin, BluePin}; // R, G, B

// Password
char Password[] = "2345";

// Lock status
int lockStatus = CLOSED;

// Input buffer
char In_buffer[256];

// RFID module
byte nuidPICC[4];
MFRC522 rfid(SS_PIN, RST_PIN);

// finger mode select
/* 1 Search
   2 Enroll
   3 Delete All 
   */

void setup()
{
  Serial.begin(9600);
  fingerSerial.begin(57600);
  unoSerial.begin(19200);
  servo_Init(servoPin, myservo);
  Buzzer_Init(noteDuration, BuzzerPin);
  RFID_Init(rfid);
  RGB_Init(RGB_pins);
  fingerPrint_Init(finger);
  LED_Init(LED_pins);
  lockStatus = CLOSED;
}

// 默认读卡模式

void loop()
{

  while (true) {
    char key = keypad.getKey();
    if (key) {
      PassMode = key - '0';
      break;
    }
  }

  switch (PassMode)
  {
  case sRFID:
  {
    // Running LED
    digitalWrite(LED_pins[0], HIGH);
    /* uno Serial message send */
    unoSerial.write('c');
    /* RFID 输入模式*/
    /* Test */
    // 找卡
    if (!rfid.PICC_IsNewCardPresent())
      return;
    // 验证NUID是否可读
    if (!rfid.PICC_ReadCardSerial())
      return;
    MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
    // 检查是否MIFARE卡类型
    if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&
        piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
        piccType != MFRC522::PICC_TYPE_MIFARE_4K)
    {
      return;
    }
    // 将NUID保存到nuidPICC数组
    for (byte i = 0; i < 4; i++)
    {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
    // 使放置在读卡区的IC卡进入休眠状态，不再重复读卡
    rfid.PICC_HaltA();

    // 停止读卡模块编码
    rfid.PCD_StopCrypto1();
    if (nuidPICC[0] == 0x33 && nuidPICC[1] == 0x10 && nuidPICC[2] == 0x52 && nuidPICC[3] == 0x97)
    {
      unoSerial.write('p');
      lockStatus = servo_turn(pos, myservo, lockStatus);
      passwordRight(RGB_pins, noteDuration, BuzzerPin);
      lockStatus = servo_turn(pos, myservo, lockStatus);
    }
    else
    {
      unoSerial.write('w');
      passwordWrong(RGB_pins, noteDuration, BuzzerPin);
    }
    RGB_Init(RGB_pins);
    break;
  }

  case sKeypad:
  {
    unoSerial.write('i');
    // Running LED
    digitalWrite(LED_pins[1], HIGH);
    /* 键盘输入密码
      错误导致蜂鸣器蜂鸣，LCD显示"Wrong", RGB亮红灯
      密码正确LCD显示"PASS!"
    */
    enterPassword(keypad, In_buffer);
    if (!inputPasswordCompared(In_buffer, Password))
    {
      unoSerial.write('p');
      lockStatus = servo_turn(pos, myservo, lockStatus);
      passwordRight(RGB_pins, noteDuration, BuzzerPin);
      lockStatus = servo_turn(pos, myservo, lockStatus);
    }
    else
    {
      unoSerial.write('w');
      passwordWrong(RGB_pins, noteDuration, BuzzerPin);
    }
  }

  case sFingerPrint:
  {
    unoSerial.write('f');
    digitalWrite(LED_pins[2], HIGH);
    /* 指纹识别模式 */
    /* Default mode is Search */

    /*test*/
    char fingerModeChoose;
    while (true) {
      fingerModeChoose = keypad.getKey();
      if (fingerModeChoose) {
        break;
      }
    }

    switch (fingerModeChoose) {
      case '1':{
        finger.getTemplateCount();
        Serial.print(finger.templateCount);
        if (getFingerprintIDez(finger, RGB_pins) == -1)
        {
          unoSerial.write('w');
          passwordWrong(RGB_pins, noteDuration, BuzzerPin);
        } else {
          unoSerial.write('p');
          passwordRight(RGB_pins, noteDuration, BuzzerPin);
        }
        RGB_Init(RGB_pins);
        break;
      }
      case '2': {
        while ((id = readnumber(keypad)) == 0);
        getFingerprintEnroll(finger, id, RGB_pins);
        break;
      }
      case '3': {
        finger.emptyDatabase();
        break;
      }
      default:{}
    }
    break;
  }

  default:{}
  }

  for (int i = 0;i < 3;i++) {
    digitalWrite(LED_pins[i], LOW);
  }
}

/* Password reset with keypad */
/// keypad listener
// void keypadEvent(KeypadEvent key)
// {
//     switch (keypad.getState())
//     {
//     case PRESSED:
//         Serial.print(key);
//         if (key == '#')
//         {
//             // clear
//         }
//         if (key == '*')
//         {
//             // confirm
//         }
//         break;
//     case RELEASED:
//         if (key == '*')
//         {
//         }
//         break;
//     case HOLD:
//         if (key == '*')
//         {
//         }
//         break;
//     }
// }
