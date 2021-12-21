#include <usrlib.h>

void RGB_color(int Rpin, int Gpin, int Bpin, int red, int green, int blue)
{
    analogWrite(Rpin, red);
    analogWrite(Gpin, green);
    analogWrite(Bpin, blue);
}

void RGB_color_blue(int RGB_pins[3])
{
    RGB_color(RGB_pins[0], RGB_pins[1], RGB_pins[2], 0, 0, 255);
}

void RGB_color_red(int RGB_pins[3])
{
    RGB_color(RGB_pins[0], RGB_pins[1], RGB_pins[2], 255, 0, 0);
}

void RGB_color_green(int RGB_pins[3])
{
    RGB_color(RGB_pins[0], RGB_pins[1], RGB_pins[2], 0, 255, 0);
}



// setup 初始化
/* LCD, servo, RGB, keypad, Buzzer */



void servo_Init(const int servoPin, Servo myservo)
{
    myservo.attach(servoPin);
}

void RGB_Init(int RGB_pins[3])
{
    for (int i = 0; i < sizeof(RGB_pins) / sizeof(const int); i++)
    {
        pinMode(RGB_pins[i], OUTPUT);
    }
    RGB_color(RGB_pins[0], RGB_pins[1], RGB_pins[2], 0, 0, 0);
}

void Buzzer_Init(int BuzzerPin, int noteDuration)
{
    pinMode(BuzzerPin, OUTPUT);
    noteDuration = 4;
}

void RFID_Init(MFRC522 rfid)
{
    SPI.begin();
    rfid.PCD_Init();
}

void LED_Init(int LED_pins[])
{
    for (int i = 0; i < 3; i++)
    {
        pinMode(LED_pins[i], OUTPUT);
    }
}

// RFID function

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
char keyRead(Keypad keypad)
{
    return keypad.getKey();
}

//舵机转向  servo rotate function
/* 接收到开门/关门信号,舵机旋转
  1.刷卡
  2.指纹
  3.键盘密码输入
*/
int servo_turn(int pos, Servo myservo, int lockStatus)
{
    if (lockStatus == CLOSED)
    {
        for (pos; pos <= 180; pos++)
        {
            myservo.write(pos);
            delay(1);
        }
        lockStatus = OPENED;
    }
    else
    {
        for (pos; pos >= 0; pos--)
        {
            myservo.write(pos);
            delay(1);
        }
        lockStatus = CLOSED;
    }
    return lockStatus;
}

//蜂鸣器模块
void buzzerSpeaker(long noteDuration, int BuzzerPin, bool num)
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



//密码错误LCD显示


// 正确密码显示


//键盘输入字符与预设密码比较
bool passwordCompare(char inputkey, char *password, int place)
{
    char *pass_p = password + place;
    if (*pass_p != inputkey)
    {
        return false;
    }
    return true;
}

// 密码错误警报
void passwordWrong(int RGB_pins[3], int noteDuration, int BuzzerPin)
{

    RGB_color_red(RGB_pins);
    buzzerSpeaker(noteDuration, BuzzerPin, WRONG);

    RGB_Init(RGB_pins);
}

// 密码正确
void passwordRight( int RGB_pins[3], int noteDuration, int BuzzerPin)
{

    RGB_color_green(RGB_pins);
    buzzerSpeaker(noteDuration, BuzzerPin, PASS);
    delay(5000);
    RGB_Init(RGB_pins);
 
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

char * enterPassword(Keypad keypad,int *clock)//Modified by JW
{
	char *pw_pointer;
    static char buff[]="!!!!";
    char* buff_pointer = buff;
    static char pw[5]="!!!!!";
    pw_pointer = pw;
    int i = 0;
    char key;

    while (((key = keyRead(keypad))) != '*'    )//�������� i<6��ԭ�򣺽���ģʽ�����1��1����������4��������*Ҳ��һ��
    {

        if((*(buff_pointer+i-1)!=key || i==0 )&& key){
          *(buff_pointer+i)= key; 
          Serial.print("Entered key is:");
          Serial.println(*(buff_pointer+i));    
          i++;
          
        }
    }
    if((key = keyRead(keypad))=='*' )//����������ֹ
    {
        Serial.println("* is be entered!");
        *clock=0;
        pw_pointer=buff_pointer;
    }
    for(int ii=0;ii<5;ii++){
      Serial.println();
      Serial.print("Entered ps in the headfile is:");
      Serial.print(*(pw_pointer+ii));
    }
    return pw_pointer;
}

//输入成功后密码比较
bool inputPasswordCompared(char *input, char *password)
{
    bool result = false;
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
int modeSelect(Keypad keypad)
{
    return keypad.getKey() - '0';
}

// 指纹识别初始化
void fingerPrint_Init(Adafruit_Fingerprint finger)
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
uint8_t readnumber(Keypad keypad)
{
    uint8_t num = 0;

    while (num == 0)
    {
        num = keyRead(keypad) - '0';
    }
    return num;
}

// Enroll fingerprint
uint8_t getFingerprintEnroll(Adafruit_Fingerprint finger, uint8_t id, int RGB_pins[3])
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
        RGB_color_blue(RGB_pins);
        delay(2000);
        RGB_Init(RGB_pins);
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
int getFingerprintIDez(Adafruit_Fingerprint finger, int RGB_pins[3])
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
        RGB_color_green(RGB_pins);
        delay(2000);
        RGB_Init(RGB_pins);
    }

    // found a match!
    return finger.fingerID;
}
