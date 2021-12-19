#include <Arduino.h>
#include <Servo.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <pitches.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>

// #define mySerial Serial1
#define OPENED HIGH
#define CLOSED LOW
#define RST_PIN 49
#define SS_PIN 48

//模式选择
#define sRFID 0
#define sKeypad 1
#define sFingerPrint 2

#define PASS 1
#define WRONG 0

// // fingerprint
// uint8_t id;
// int LED_pins[2] = {36, 37};

// LCD
// const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// // Servo
// Servo myservo;
// const int servoPin = 26;
// int pos = 0;

// // Speaker
// int noteDuration = 4, BuzzerPin = 25;

// // RGB
// const int RedPin = 22, GreenPin = 23, BluePin = 24;
// const int RGB_pins[] = {RedPin, GreenPin, BluePin}; // R, G, B

// Keypad
// const byte ROWS = 4;
// const byte COLS = 3;

// char keys[ROWS][COLS] = {
//     {'1', '2', '3'},
//     {'4', '5', '6'},
//     {'7', '8', '9'},
//     {'*', '0', '#'}};

// byte rowPins[ROWS] = {2, 3, 4, 5};
// byte colPins[COLS] = {6, 7, 8};
// Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// // Password
// char Password[] = "2345";

// // RFID module
// byte nuidPICC[4];
// MFRC522 rfid(SS_PIN, RST_PIN);


// RGB control
void RGB_color(int Rpin, int Gpin, int Bpin, int red, int green, int blue);
void RGB_color_blue(int RGB_pins[3]);
void RGB_color_red(int RGB_pins[3]);
void RGB_color_green(int RGB_pins[3]);

void LCDPasswordDisplaySet(int line, int offset);

// RFID Serial output
void printHex(byte *buffer, byte bufferSize);
void printDec(byte *buffer, byte bufferSize);

//Keypad
char keyRead(Keypad keypad);
void keypadEvent(KeypadEvent key);

// servo return lock status
int servo_turn(int pos, Servo servo, int lockStatus);

// LCD
void buzzerSpeaker(long noteDuration, int BuzzerPin, bool num);
void defaultDisplay(LiquidCrystal lcd);
void LCDPasswordWrong(LiquidCrystal lcd);
void LCDPasswordRight(LiquidCrystal lcd);
boolean passwordCompare(char inputkey, char *password, int place);
void passwordWrong(LiquidCrystal lcd, int RGB_pins[3], int noteDuration, int BuzzerPin);
void passwordRight(LiquidCrystal lcd, int RGB_pins[3], int noteDuration, int BuzzerPin);

// fingerprint
uint8_t readnumber(Keypad keypad);
uint8_t getFingerprintEnroll(Adafruit_Fingerprint finger, uint8_t id, int LED_pins[]);
int getFingerprintIDez(Adafruit_Fingerprint finger, int LED_pins[]);

// Function
int _strlen(char *str);
void enterPassword(Keypad keypad, char *In_buffer);
boolean inputPasswordCompared(char *input, char *password);
int modeSelect(Keypad keypad);
int openLock(int pos, Servo myservo, int lockStatus);
int closeLock(int pos, Servo myservo, int lockStatus);

// Inititialize
void LCD_Init(LiquidCrystal lcd);
void servo_Init(const int servoPin, Servo myservo);
void RGB_Init(int RGB_pins[3]);
void Buzzer_Init(int BuzzerPin, int noteDuration);
void RFID_Init(MFRC522 rfid);
void LED_Init(int LED_pins[2]);
void fingerPrint_Init(Adafruit_Fingerprint finger);