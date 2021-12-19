#include <SoftwareSerial.h>
SoftwareSerial softSerial(1,0);
//This is uno's code
//uno's pin 0 connect to mega 's pin D1
//uno's pin 1 connect to mega 's pin D0

void setup() {
  softSerial.begin(9600); //Can't understand that this line stander for what.

}


void loop() {
  delay(2000);
}
