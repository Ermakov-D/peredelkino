#include "mariamole_auto_generated.h"
#include <OLED_I2C.h>
#include <DS3231.h>
#include <DHT.h>

#include "floatToString.h"
#include "EEPROM_float_rw.h"

#define DHTPIN 2
#define DHTTYPE DHT22
#define LIGHTPIN 13
#define RELAYPIN 12
//#define SOLID_I 0


DS3231 rtc_time(SDA,SCL);
OLED  myOLED(SDA, SCL, 8);
DHT term_sensor(DHTPIN,DHTTYPE);

extern uint8_t MegaNumbers[];
extern uint8_t MediumNumbers[];
extern uint8_t SmallFont[];
extern uint8_t RusFont[];

// int show=0;
float maxTerm = -50;
float minTerm = 50;

boolean AM = false;
boolean PM = false;
boolean RelayStatus = false;

// Conversion template 
char buffer[10];

//values for loop 
int show = 1;
unsigned long currentTime;
unsigned long loopTime;

// EEPROM values
// 0 -  (maxTerm)
// 4 -  (minTerm)
// 5 - morning
// 6 - night

void setup() {
  myOLED.begin();
  rtc_time.begin();
  term_sensor.begin();
  
  pinMode(LIGHTPIN,OUTPUT);
  pinMode(RELAYPIN,OUTPUT);
	//Serial.begin(9600);
	//Serial.println("Start");
}

void loop() {
  Time time_now;
  //Serial.println("loop");

  time_now = rtc_time.getTime();
  if (time_now.hour == 8 && time_now.min >=0 && time_now.min < 20) {
  // digitalWrite(LIGHTPIN,HIGH);
   digitalWrite(RELAYPIN,LOW);
   AM = true;RelayStatus = true;EEPROM.write(5,byte(AM));
  } else if (time_now.hour == 20 && time_now.min >=0 && time_now.min < 20) {
  // digitalWrite(LIGHTPIN,HIGH);
   digitalWrite(RELAYPIN,LOW);
   PM = true;RelayStatus = true;EEPROM.write(6,byte(PM));   
  } else {
  // digitalWrite(LIGHTPIN,LOW);
   digitalWrite(RELAYPIN,HIGH);
   RelayStatus = false;      
  }
  
  if (time_now.hour == 0 && time_now.min == 1) {
   AM = false;EEPROM.write(5,byte(AM));
   PM = false;EEPROM.write(6,byte(PM));
   maxTerm = -50;
   minTerm = 50;   
  delay (60000);
  }

  currentTime = millis();
  if (currentTime >= (loopTime + 3000)) {	
	  loopTime = currentTime;
	  switch (show) {
		  case 1:
				show_time();
				show ++;
				break;
		  case 2:
				show_term();
				show++;
				break;
		  case 3:
				show_hum();
				show++;
				break;
		  case 4:
				show_solid();
				show++;
				break;
		  case 5:
				show_stats();
				show = 1;
				break;
	  }
  }
	  
  /*
  show_time(); delay(2000);  
  show_term(); delay(2000);  
  show_hum(); delay (2000);
  show_solid(); delay(2000);
  show_stats(); delay(2000);
*/
 }

void show_time() {
  //Serial.println("Time");
	myOLED.setFont(SmallFont);
  myOLED.print(rtc_time.getDOWStr(), CENTER, 0);   // ??????????? ??? ??????
  String stringOne = rtc_time.getTimeStr();
  myOLED.print(rtc_time.getDateStr(), CENTER, 57);      // ??????????? ????
  myOLED.setFont(MegaNumbers);
  myOLED.print(stringOne.substring(0,2), 4, 12);   // ??????????? ?????
  myOLED.print("/", 51, 12);                       // ??????????? ?????????
  myOLED.print(stringOne.substring(3,5), 75, 12);  // ??????????? ?????
    
  myOLED.update();  
}

void show_term() {
	Serial.println("Term");
  float currentTemperature = term_sensor.readTemperature(); 
  String stringOne = rtc_time.getTimeStr();

  if (currentTemperature > maxTerm)  { 
	  maxTerm=currentTemperature;
	  EEPROM_float_write(0,maxTerm);
  }
  if (currentTemperature < minTerm)  {
	  minTerm=currentTemperature;
	  EEPROM_float_write(4,minTerm);
  }
 
  myOLED.clrScr();
  show_title("Ntvgthfnehs");
  myOLED.setFont(SmallFont);
  myOLED.print("Min         Max",CENTER,35); 
  myOLED.print(floatToString(buffer,minTerm,2),0,45); 
  myOLED.print(floatToString(buffer,maxTerm,2),95,45);
  myOLED.setFont(MediumNumbers);
  myOLED.print(floatToString(buffer,currentTemperature,2),CENTER,12);
    
  myOLED.update();
  myOLED.clrScr();
}

void show_hum() {
	Serial.println("Hum");
  float humidity = term_sensor.readHumidity(); 
  // String stringOne = rtc_time.getTimeStr();
  
  myOLED.clrScr();
  show_title("Dkf;yjcnm");
  myOLED.setFont(MegaNumbers);
  myOLED.print(floatToString(buffer,humidity,2),CENTER,12);
  myOLED.update();
  myOLED.clrScr();
}

void show_solid(){
	Serial.println("Solid");
  // 16 368 bytes -- 1 653
  int Solid = analogRead(0);
  
  show_title("Dkf;yjcnm gjxds");
  myOLED.setFont(MediumNumbers);
	Serial.println(String(Solid,DEC));
  myOLED.print(String(Solid,DEC),CENTER,12);
  myOLED.setFont(RusFont);
  if (Solid <= 450) myOLED.print("JXTYM DKF:YJ",CENTER,40);
  else if( Solid>450 && Solid<700) myOLED.print("DKF:YFZ GJXDF",CENTER,40);
  else myOLED.print("CE{FZ GJXDF",CENTER,40);
  myOLED.update();
  myOLED.clrScr();
}

void show_stats(){
	//Serial.println("Stats");
   AM=EEPROM.read(5);
   PM=EEPROM.read(6);

   show_title("Ghjdthrf");
   myOLED.print("Gjkbd enhjv   -",LEFT,12);
   if (AM) myOLED.print("LF",RIGHT,12);
   else myOLED.print("YTN",RIGHT,12);
   myOLED.print("Gjkbd dtxthjd -",LEFT,22);
   if (PM) myOLED.print("LF",RIGHT,22);
   else myOLED.print("YTN",RIGHT,22);
   myOLED.print("Htkt -",LEFT,32);
   if (PM) myOLED.print("Drk",RIGHT,32);
   else myOLED.print("Dsrk",RIGHT,32);


   myOLED.update();
   myOLED.clrScr(); 
}
void show_title(String Title){
	Serial.println("Title");
  String stringOne = rtc_time.getTimeStr();
  
  myOLED.clrScr();
  myOLED.setFont(SmallFont);
  myOLED.print(stringOne.substring(0,5),98,0);
  myOLED.print(rtc_time.getDateStr(),0,0);
  myOLED.setFont(RusFont);
  myOLED.print(Title,CENTER,57);  
}

