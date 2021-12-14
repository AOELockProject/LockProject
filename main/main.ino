#include <libraries.h>

// RFID 读取显示

void setup()
{
  Serial.begin(9600);
  servo_Init(servoPin);
  Buzzer_Init(noteDuration, BuzzerPin);
  RFID_Init();
  RGB_Init();
}

// 默认读卡模式

void loop()
{
  PassMode = modeSelect();
  switch (PassMode)
  {
  case sRFID:
  {
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
      Serial.println("不支持读取此卡类型");
      return;
    }
    // 将NUID保存到nuidPICC数组
    for (byte i = 0; i < 4; i++)
    {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
    Serial.print("十六进制UID:");
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    Serial.print("十进制UID:");
    printDec(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();

    // 使放置在读卡区的IC卡进入休眠状态，不再重复读卡
    rfid.PICC_HaltA();

    // 停止读卡模块编码
    rfid.PCD_StopCrypto1();
    if (nuidPICC[0] == 0x33 && nuidPICC[1] == 0x10 && nuidPICC[2] == 0x52 && nuidPICC[3] == 0x97)
    {
      Serial.println("身份确认解锁成功!");
      RGB_color_green();
      buzzerSpeaker(noteDuration, BuzzerPin, 1);
      servo_turn();
      delay(5000);
      servo_turn();
    }
    else
    {
      Serial.println("非系统用户，无法解锁!");
      passwordWrong();
    }
    delay(1000);
    RGB_Init();
    break;
  }

  case sKeypad:
  {
    /* 键盘输入密码
      错误导致蜂鸣器蜂鸣，LCD显示"Wrong", RGB亮红灯
      密码正确LCD显示"PASS!"
    */
    enterPassword(_strlen(Password));
    if (!inputPasswordCompared(In_buffer, Password))
    {
      passwordRight();
    } else {
      passwordWrong();
    }
  }

  case sFingerPrint:
  {
    /*指纹识别模式*/


    break;
  }
  default: {}
  }
}