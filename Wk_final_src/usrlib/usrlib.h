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

// RGB control
void RGB_color(int Rpin, int Gpin, int Bpin, int red, int green, int blue);
void RGB_color_blue(int RGB_pins[3]);
void RGB_color_red(int RGB_pins[3]);
void RGB_color_green(int RGB_pins[3]);

// RFID Serial output
void printHex(byte *buffer, byte bufferSize);
void printDec(byte *buffer, byte bufferSize);

//Keypad
char keyRead(Keypad keypad);
void keypadEvent(KeypadEvent key);

// servo return lock status
int servo_turn(int pos, Servo servo, int lockStatus);

// Password
void buzzerSpeaker(long noteDuration, int BuzzerPin, bool num);
bool passwordCompare(char inputkey, char *password, int place);
void passwordWrong(int RGB_pins[3], int noteDuration, int BuzzerPin);
void passwordRight(int RGB_pins[3], int noteDuration, int BuzzerPin);

// fingerprint
uint8_t readnumber(Keypad keypad);
uint8_t getFingerprintEnroll(Adafruit_Fingerprint finger, uint8_t id, int RGB_pins[3]);
int getFingerprintIDez(Adafruit_Fingerprint finger, int RGB_pins[3]);

// Function
int _strlen(char *str);
void enterPassword(Keypad keypad, char *In_buffer);
bool inputPasswordCompared(char *input, char *password);
int modeSelect(Keypad keypad);
int openLock(int pos, Servo myservo, int lockStatus);
int closeLock(int pos, Servo myservo, int lockStatus);

// Inititialize
void servo_Init(const int servoPin, Servo myservo);
void RGB_Init(int RGB_pins[3]);
void Buzzer_Init(int BuzzerPin, int noteDuration);
void RFID_Init(MFRC522 rfid);
void LED_Init(int LED_pins[2]);
void fingerPrint_Init(Adafruit_Fingerprint finger);