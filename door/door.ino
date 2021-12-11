#include <SoftwareSerial.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h> 
#include<EEPROM.h>
#include <Servo.h>
#include <rfid.h>  

RFID rfid;
uchar serNum[5];
Servo myServo;
LiquidCrystal_I2C lcd(0x27,16,2);  
char passwd[4];
char initial_password[4],new_password[4];
int i=0;

int j=0;
int fmq=12;

char key_pressed=0;
const byte rows = 4;
const byte columns = 4;
char hexaKeys[rows][columns] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};
byte row_pins[rows] = {9,8,7,6};
byte column_pins[columns] = {5,4,3,2};       //键盘的引脚设定
Keypad keypad_key = Keypad( makeKeymap(hexaKeys), row_pins, column_pins, rows, columns);

//初始密码的设定
void initialpassword(){
  for(int j=0;j<4;j++)
   EEPROM.write(j, j+49);
  for(int j=0;j<4;j++)
    initial_password[j]=EEPROM.read(j);
}

void setup()
{
  myServo.attach(13);
  myServo.write(0);
  Serial.begin(9600);

  pinMode(fmq, OUTPUT);                //设定引脚模式为输出                                                          
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Enter Password:");
  lcd.setCursor(0,1);
  initialpassword();
  rfid.begin(10, A2, A1,A0 , A3, 11);   //rfid.begin
  delay(100); 
  rfid.init(); 
  delay(2000);     
}
//按下的键存储在“ 密码 ”变量中，当按下键时，这些键显示在LCD上。然后这些键将与存储在EEPROM中的初始密码相匹配。如果按下的键与初始密码匹配，则锁定将打开，并且LCD上将打印“已接受通过”。如果密码不匹配，则会要求再次输入密码。
void loop()
{
  digitalWrite(fmq, HIGH); 
  uchar status;
  uchar str[MAX_LEN];
  status = rfid.request(PICC_REQIDL, str);
 
while(status== MI_OK)
{  
 
  //显示卡的类型
  rfid.showCardType(str);
  //防止冲突，返回4字节的序列号的卡片
  status = rfid.anticoll(str);

 
Serial.print("The card's number is: ");
    lcd.clear();
    lcd.setCursor(0,0);           //LCD的坐标：0列，0行
    lcd.print(" ID: ");
    memcpy(serNum, str, 5);       
    rfid.showCardID(serNum);      
    Serial.println();
    //确认IC卡的ID是否为 04DFBD2B
    uchar* id = serNum;
    if(id[0]==0xAC && id[1]==0xAF && id[2]==0x3B && id[3]==0xA9) 
    { //该卡号可以更改为自己的IC卡号，包括校园卡等
      myServo.write(90);         //舵机旋转至90°
      Serial.println("Hello MAN YI!");
      lcd.setCursor(0,1);         //显示的坐标：0列，1行
      lcd.print("Welcome to home!");
      delay(2000);
      lcd.clear();                //LCD清屏
      myServo.write(0);
      delay(2000);
      break;
  }
    else
    { //若卡号与上述不符
      lcd.setCursor(0,1);
      lcd.print("Please try again!");
      digitalWrite(fmq, LOW); 
      delay(250);
      lcd.clear();
    }
}
  

  if (Serial.available()){
   int value=Serial.read();

    if (value == '1'){
    myServo.write(0);
    delay(1000);
      }

       if (value == '2'){
    myServo.write(90);
    delay(1000);
      }
}
  lcd.setCursor(0,0);
  lcd.print("Enter Password:");
  lcd.setCursor(0,1);
  
  myServo.write(0);
 
  key_pressed = keypad_key.getKey();

if(j<3){
  if (key_pressed)
  {
passwd[i++]=key_pressed;
lcd.print(key_pressed);
while(key_pressed=='*')
{
    change();
    i=0;
}
      }
  if(i==4)
  {
    delay(200);
    for(int j=0;j<4;j++)
      initial_password[j]=EEPROM.read(j);
    if(!(strncmp(passwd, initial_password,4)))              //比较输入密码与当前密码是否一致
    {
      lcd.clear();
      lcd.print("Pass Accepted");
     
      myServo.write(90);
      delay(2000);
      lcd.clear();
      lcd.print("Enter Password:");
      lcd.setCursor(0,1);
      i=0;

    }
    else
    {
      myServo.write(0);
      lcd.clear();
      lcd.print("Wrong Password");
      digitalWrite(fmq, LOW); 
      delay(500);
      digitalWrite(fmq, HIGH); 
      lcd.clear();
      lcd.print("Enter Password");
      lcd.setCursor(0,1);
      i=0;
    j++;
}
  }

}
else{
  lcd.clear();
  lcd.print("Wait 1 minute!");
  delay(60000);
  j=0;
  return;
  }

}
//如果按下'*'键，它将调用' change（） '函数。在更改功能中，它会要求您输入当前密码。如果当前密码正确，则会要求您输入新密码。输入新密码后，密码将被更改。
void change()                   //修改密码
{
  int j=0;
  lcd.clear();
  lcd.print("Current Password");
  lcd.setCursor(0,1);
  while(j<4)
  {
    char key=keypad_key.getKey();
    if(key)

    {
      new_password[j++]=key;
      lcd.print(key);  
    }
    key=0;
  }
  delay(500);

  if((strncmp(new_password, initial_password, 4)))
  {
    lcd.clear();
    lcd.print("Wrong Password");
    lcd.setCursor(0,1);
    lcd.print("Try Again");
    delay(1000);
  }
  else
  {
    j=0;
    lcd.clear();
    lcd.print("New Password:");
    lcd.setCursor(0,1);
    while(j<4)
    {

      char key=keypad_key.getKey();
      if(key)
      {
        initial_password[j]=key;
        lcd.print(key);
        EEPROM.write(j,key);
        j++;

      }
    }
    lcd.print("Pass Changed");
    delay(1000);
  }
  lcd.clear();
  lcd.print("Enter Password");
  lcd.setCursor(0,1);
  key_pressed=0;
}
