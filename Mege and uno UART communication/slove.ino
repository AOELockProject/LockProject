#include <SoftwareSerial.h> 
//This is mega's code
//mega's pin 1 connect to uno 's pin D0
//mega's pin 0 connect to uno 's pin D1

void setup(){
    
    Serial.begin(9600);//It's means opened the uno   

   
    }

void loop(){
    
    Serial.write("10 ");//You can enter there what you want to send to uno
    delay(2000);
}
