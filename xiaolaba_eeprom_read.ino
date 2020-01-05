
/*
 * Dump 24C16 to screen
 * 
 * xiaolaba, 2014-JAN-14, init, uses library, https://github.com/yazug/EEPROM24C04_16, modified.
 * xiaolaba, 2020-JAN-04, revise dump format to C-code array ready 
 * 
 * Arduino IDE 1.8.9 (Windows Store 1.8.21.0)
 * 
 * hardware connection,
 * 
 * 24C04 pin#   <-> Ardunino Nano pin# (Atmega168)
 * 1, A0        <-> GND
 * 2, A1        <-> GND
 * 3, A2        <-> GND
 * 4, GND       <-> GND
 * 5, SDA       <-> A4, SDA(Mega168/328)
 * 6, SCL       <-> A5, SCL(Mega168/328)
 * 7, /WP       <-> GND, always enable write
 * 8, VCC, +5V  <-> VCC, +5V
 * 
 * Serial, 115200 baud
 */


#define EEPROM_ADDR_Ax 0b000 // A2, A1, A0
#define EEPROM_ADDR (0b1010 << 3) + EEPROM_ADDR_Ax

// 24c01, 01 Kbit  = 1*1024/8 = 128 byte, array 16x8, length 0x80
// 24c02, 02 Kbit  = 2*1024/8 = 256 byte, array 16x16, length 0x100
// 24c04, 04 Kbit  = 4*1024/8 = 512 byte, array 16x32, length 0x200
// 24c08, 08 Kbit  = 8*1024/8 = 1024 byte, array 16x64, length 0x400
// 24c16, 16 Kbit = 16*1024/8 = 2048 byte, array 16x126, length 0x800

#define Kbit 16
#define EEPROM_SIZE Kbit*1024/8 


String command;
int i =0;
char buf[17]={'\0','\0','\0','\0',
              '\0','\0','\0','\0',
              '\0','\0','\0','\0',
              '\0','\0','\0','\0',
              '\0'};  //null terminated for Serial.print()
              

// RAM, 24C02, dump record, no uses
//--------------------------------------------------------------------------------------------
//  00   01   02   03   04   05   06   07   08   09   0A   0B   0C   0D   0E   0F  // OFFSET
//--------------------------------------------------------------------------------------------
const char* const EEPROM_DATA[] PROGMEM = {
  0xF0,0xF0,0xF0,0xF0,0xF0,0x53,0x01,0xFE,0xA2,0x00,0xA0,0x3D,0xF0,0xF0,0xF0,0xF0, // 0000000F .....S.....=....
  0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0,0xF0, // 0000001F ................
  0x43,0x4E,0x34,0x33,0x44,0x44,0x53,0x31,0x36,0x50,0x30,0x35,0x52,0x51,0x00,0x00, // 0000002F CN43DDS16P05RQ..
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0000003F ................
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0000004F ................
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0000005F ................
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0000006F ................
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0000007F ................
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0000008F ................
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 0000009F ................
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xAC,0xED,0x00,0x00,0x00, // 000000AF ................
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 000000BF ................
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // 000000CF ................
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xC0,0xC1,0x00,0x00,0x00, // 000000DF ................
  0x00,0x00,0x00,0x01,0xC0,0xC1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xD0,0x00, // 000000EF ................
  0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x01,0x00,0x01,0x00,0x64,0x01,0x00,0x01,0x00,0x00, // 000000FF ..........d.....

};
//--------------------------------------------------------------------------------------------


#include "Eeprom24C04_16.h"

#include <Wire.h>

static Eeprom24C04_16 eeprom(EEPROM_ADDR);

//bug, limit to 1k bit
/*
void eeprom_i2c_write(byte device_address, byte from_addr, byte data) {
  Wire.beginTransmission(device_address);
  Wire.write(from_addr);
  //Wire.write((int)from_addr  >> 8); //MSB
  //Wire.write((int)from_addr &0xff); //LSB  
  Wire.write(data);
  Wire.endTransmission();
}

byte eeprom_i2c_read(int device_address, int from_addr) {
  Wire.beginTransmission(device_address);  //sending dummy write command, to load target address
  Wire.write(from_addr);
  //Wire.write((int)from_addr  >> 8); //MSB
  //Wire.write((int)from_addr &0xff); //LSB
  Wire.endTransmission();

  Wire.requestFrom(device_address, 1); // read the eeprom byte 
  if(Wire.available())
    return Wire.read();
  else
    return 0xFF;
}

*/


void setup() {
  Wire.begin();
  Serial.begin(115200);
  ShowBanner();

  eeprom.initialize();  // Initiliaze EEPROM library

  //////////
  // dummy write test, not used
  //////////
/*
  for(int i = 0; i < 10; i++) {
    eeprom.writeByte(i, '0'+i);
    delay(100);
  }

  for(int i = 0x7f6; i < 0x800; i++) {
    eeprom.writeByte(i, 'a'+i-0x7f6);
    delay(100);
  }

  Serial.println(F("offset 0x00 & 0x7f6, 10 bytes written as '0123456789' & 'abcdefghij'"));
  Serial.println(F("read result shoul be 0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39, ... \n"));
*/


}

void ShowBanner (){
  Serial.println(F("24Cx I2C EEPROM reader ready! r = Read command"));
  Serial.print  (F("Device ADDR = 0x")); Serial.println(EEPROM_ADDR, HEX);
  Serial.print  (Kbit); Serial.print(F(" Kbit = ")); Serial.print(EEPROM_SIZE,DEC); Serial.println(F(" Byte\n")); 
  Serial.println(F("xiaolaba, 2014-JAN-14"));
  Serial.println(F("dumping as C-code array ready, 2020-JAN-04\n"));
}

void ShowArrayHeader (){
  Serial.println(F("const char* const EEPROM_DATA[] PROGMEM = {")); 
}

void ShowArrayTrailer (){
  Serial.println("};"); 
}

void ShowLableOffset (){
  // lable line, show offset
  Serial.print(F("//  "));
  for(i = 0; i <= 15; i++) {
    Serial.print("0"); Serial.print(i, HEX);
    if (i < 15) {Serial.print(F("   "));} else { Serial.println(F("  // OFFSET")); }
  }

}

void ShowMarker (){
  Serial.print(F("//-------------"));  
  for(i = 0; i <= 15; i++) {
    Serial.print(F("----"));
    if (i < 15) {Serial.print("-");} else { Serial.println(); }
  }
}


void DumpEEPROM() {

      int j =0;
      
      ShowBanner();
      ShowMarker();
      ShowLableOffset();
      ShowMarker();
      ShowArrayHeader();

      
      // dump EEPROM, every byte
      for(i = 0; i < EEPROM_SIZE; i++) {
        // format data array, two spaces leading at each new line
        if ( (i&0xf)== 0) Serial.print("  ");
        
        // read EEPROM, a byte
        byte r = eeprom.readByte(i);

/*
        // dump data as ASCII, for debug only, not used
        if (r < 0x20 || r > 0x7f) {
          Serial.print('.'); //not ASCII printable
        } else {
          Serial.print(char(r));  // print ASCII
        } Serial.print(" ");
*/
        // dump data as HEX
        Serial.print("0x"); if (r < 0x10) Serial.print("0"); // padding 0x0?
        Serial.print(r, HEX);
        Serial.print(",");  // comma seperator

        //store eerpom data to buf as ASCII, later will print to serial/screen
        if (r < 0x20 || r > 0x7f) {
          buf[j]=('.'); //not ASCII printable, replaced by "."
        } else {
          buf[j]= r;  // printable char
        }
        j++;  //advance pointer, for next byte storage
 
        // each 16 bytes, show address, padding for 16bit format
        if ( (i&0xf)== 0xf) {
            if (i >= 0x00000000 && i < 0x00000010 ) Serial.print(F(" // 0000000"));
            if (i >= 0x00000010 && i < 0x00000100 ) Serial.print(F(" // 000000"));
            if (i >= 0x00000100 && i < 0x00001000 ) Serial.print(F(" // 00000"));
            if (i >= 0x00001000 && i < 0x00010000 ) Serial.print(F(" // 0000"));   
            if (i >= 0x00010000 && i < 0x00100000 ) Serial.print(F(" // 000")); 
            if (i >= 0x00100000 && i < 0x01000000 ) Serial.print(F(" // 00")); 
            if (i >= 0x01000000 && i < 0x10000000 ) Serial.print(F(" // 0"));
            Serial.print(i, HEX); Serial.print(" ");  // print address

            j=0;  //reset buf pointer for new line
            Serial.print(buf);  // print ASCII buf of this whole line of eeprom data, null terminated buf
            
            Serial.print("\n");  //change to new line
        }
        //delay(10);
      }
  
      // finish dump EEPROM content
      ShowArrayTrailer ();
      ShowMarker(); 
  
}

void loop() {

  if(Serial.available()) {
    
    command = Serial.readStringUntil('\n');

    if (command == "r") DumpEEPROM();

  } 
}

// END of program
