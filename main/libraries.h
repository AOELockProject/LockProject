#include <Servo.h>
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <pitches.h>
#include <SPI.h>
#include <MFRC522.h>

#define OPENED HIGH
#define CLOSED LOW
#define RST_PIN 49
#define SS_PIN 48
//模式选择
#define sRFID 0
#define sKeypad 1
#define sFingerPrint 2

int PassMode = sRFID;

// LCD
const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// door Status
boolean doorStatus = OPENED;

// Servo
Servo myservo;
const int servoPin = 26;
int pos = 0;

// Speaker
int noteDuration = 4, BuzzerPin = 25;
// RGB
const int RedPin = 22, GreenPin = 23, BluePin = 24;
const int RGB_pins[] = {RedPin, GreenPin, BluePin}; // R, G, B

// Keypad
const byte ROWS = 4;
const byte COLS = 3;

char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Password
char Password[] = "2345";

// Input buffer
char In_buffer[256];

// RFID module
byte nuidPICC[4];
MFRC522 rfid(SS_PIN, RST_PIN);

// RGB control
void RGB_color(int red, int green, int blue);
void RGB_color_blue();
void RGB_color_red();
void RGB_color_green();

void LCDPasswordDisplaySet(int line, int offset);

// RFID Serial output
void printHex(byte *buffer, byte bufferSize);
void printDec(byte *buffer, byte bufferSize);

//Keypad
char keyRead();
void keypadEvent(KeypadEvent key);

// servo 
void servo_turn();

// LCD

void buzzerSpeaker(long noteDuration, int BuzzerPin, boolean num);
void defaultDisplay();
void LCDPasswordWrong();
void LCDPasswordRight();
boolean passwordCompare(char inputkey, char *password, int place);
void passwordWrong();
void passwordRight();
int _strlen(char *str);

void enterPassword(int length);
boolean inputPasswordCompared(char *input, char *password);
int modeSelect();

// Inititialize
void LCD_Init();
void servo_Init(const int servoPin);
void RGB_Init();
void keypad_Init();
void Buzzer_Init(int BuzzerPin, int noteDuration);
void RFID_Init();