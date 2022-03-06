#include <EEPROM.h>
#include "lib\iarduino_RTC.h"

#include "EEPROM_ADR.h"
#include "Warning.h"
#include "Sensors.h"
#include "main.h"
#include "LCDdisplay.h"

LiquidCrystal_I2C lcd(0x3f,20,4);


char buffer[16];
byte EnableKey = 1;
boolean EncoderRead = LOW;
int StatusMenu = 0;                 // Текущее положение меню
boolean StatMenu;                   // Вывод\обновления меню экрана. Значения 1\0
boolean LightLCDEnable = false;     // Включена ли подсветка экрана. Значения 1\0 (включена\выключена)


byte Signal_OFF[8] = {				// 99 dBm
	B11111,
	B00000,
	B01001,
	B00110,
	B01001,
	B00000,
	B10000 };
byte Signal_Marginal[8] = {			// 31 dBm
	B11111,
	B00000,
	B00000,
	B00000,
	B00000,
	B01000,
	B11000 };
byte Signal_OK[8] = {				// 2-30 dBm
	B11111,
	B00000,
	B00000,
	B00000,
	B00100,
	B01100,
	B11100 };
byte Signal_Good[8] = {				// 1 
	B11111,
	B00000,
	B00000,
	B00010,
	B00110,
	B01110,
	B11110 };
byte Signal_Excellent[8] = {		// 0
	B11111,
	B00000,
	B00001,
	B00011,
	B00111,
	B01111,
	B11111 };
	

byte TempChar[8] = {                // Значок градуса
	B00111,
	B00101,
	B00111,
	B00000,
	B00000,
	B00000,
	B00000 };
/*	
byte BatteryChar0[8] = {                // Батарейка 0% заряда
	B01110,
	B10001,
	B10001,
	B10001,
	B10001,
	B10001,
	B01111 };

byte BatteryChar20[8] = {                // Батарейка 20% заряда
	B01110,
	B10001,
	B10001,
	B10001,
	B10001,
	B11111,
	B01111 };

byte BatteryChar40[8] = {                // Батарейка 40% заряда
	B01110,
	B10001,
	B10001,
	B10001,
	B11111,
	B11111,
	B01111 };
	
byte BatteryChar60[8] = {                // Батарейка 60% заряда
	B01110,
	B10001,
	B10001,
	B11111,
	B11111,
	B11111,
	B01111 };

byte BatteryChar80[8] = {                // Батарейка 80% заряда
	B01110,
	B10001,
	B11111,
	B11111,
	B11111,
	B11111,
	B01111 };
	
byte BatteryChar100[8] = {                // Батарейка 100% заряда
	B01110,
	B11111,
	B11111,
	B11111,
	B11111,
	B11111,
	B01111 };
*/

void InitializingLCDicons(){			// Инициализация значков для LCD экрана
	lcd.createChar(1, TempChar);		// Инициализация значка температуры
	lcd.createChar(2, Signal_OFF);
	lcd.createChar(3, Signal_Marginal);
	lcd.createChar(4, Signal_OK);
	lcd.createChar(5, Signal_Good);
	lcd.createChar(6, Signal_Excellent);
}


void UpdateMenu(){
	switch (StatusMenu){
		case 1:
			WindowMenu(1, 1);
			break;
		case 2:
			WindowMenu(2, 1);
			break;
		case 3:
			WindowMenu(3, 1);
			break;
		case 4:
			WindowMenu(4, 1);
			break;
	}		
}


void ViewSignalLevel(byte _Level){
	lcd.setCursor(19,0);
	if(_Level == 0){
		lcd.print(char(6));			// 	lcd.createChar(6, Signal_Excellent);
	}
	else if(_Level == 1){
		lcd.print(char(5));			// 	lcd.createChar(5, Signal_Good);
	}
	else if(2 <= _Level <= 30){
		lcd.print(char(4));			// 	lcd.createChar(4, Signal_OK);
	}
	else if(_Level == 31){
		lcd.print(char(3));			// 	lcd.createChar(3, Signal_Marginal);
	}
	else if(_Level == 99){
		lcd.print(char(2));			// 	lcd.createChar(2, Signal_OFF);
	}
}



void WindowMenu(byte NumberMenu, byte Update){   
/*   
 *    StatMenu - номер меню
 *    Update - выводить статическую информацию или динамических данных (0\1)
 */
	StatusMenu = NumberMenu;
	lcd.setCursor(0,0);
	if (NumberMenu == 0){
		lcd.backlight();
		lcd.print(F("Controller Run  "));
		lcd.setCursor(0,1);
		lcd.print(Short_VersionFirmware);
	}
	else if(NumberMenu == 1){				// Первый экран   
		if(Update == 0){					// Выводить стартовую информацию
			if(EEPROM.read(E_ModeController) == 1){
				lcd.print(F("Automatic"));        // "Automatic"
				lcd.print(F("                ")); // "                "
			}
			else{ 
				lcd.print(F("Manual"));    // "Manual"
				lcd.print(F("                ")); // "                "
			}
		lcd.setCursor(15,0);
		lcd.print(F("v"));
		lcd.setCursor(0,1);
		lcd.print(RealValueSensors[SENSOR_2][VALUE_1]);    
		lcd.print(char(1));
		lcd.print(F("C"));     // "C"
		lcd.print(F("                ")); // "                "
		lcd.setCursor(6,1);
		if(HumidityAirOffline == false){
			lcd.print(F("   "));
			lcd.print(F("%"));
			lcd.print(F("                ")); // "                "
		}
		else lcd.print(F("Error"));
    }
    else if(Update == 1){                 // Обновление динамических данных
      lcd.setCursor(10,0);
      lcd.print(VCC);
      lcd.setCursor(0,1);
      lcd.print(RealValueSensors[SENSOR_8][VALUE_1]);
      lcd.setCursor(6,1);
      if(HumidityAirOffline == true){
		  lcd.print(F(" "));
		lcd.print(F("Off  "));
      }/*
      else if(EEPROM.read(E_QuantityErrorReadHumidityAir) <= EEPROM.read(E_MaxQuantityErrorForSwitchError)){
        //lcd.print(HumidityAir);
        lcd.print(F("%  "));
      }
      else lcd.print(F("Error"));*/
      lcd.setCursor(11,1);
      lcd.print(time.gettime("H:i"));
    }
  }     
  else if(NumberMenu == 2){
    lcd.setCursor(0,0);
    /*if(RealTempLeftUp == 255){
      lcd.print(F("Error    "));
    }
    else{ 
      lcd.print(RealTempLeftUp);
      lcd.print(char(1));
      lcd.print(F("C  "));    // "C"
      lcd.print(F("          "));
    }            
    lcd.setCursor(0,1);
    if(RealTempLeftDown == 255){
      lcd.print(F("Error    "));
    }
    else{ 
      lcd.print(RealTempLeftDown);
      lcd.print(char(1));
      lcd.print(F("C  "));    // "C" 
      lcd.print(F("          "));
    }     */                                                       
  }
  else if (NumberMenu == 3){
    lcd.setCursor(0,0);
    lcd.print(F("Light Panel     "));
    lcd.setCursor(0,1);
    lcd.print(F("VCC:"));
    //lcd.print(Ext_VCC);
    lcd.print(F("  "));
    lcd.setCursor(10,1);
    lcd.print(F("I:"));
    //lcd.print(Ext_I);
    lcd.print(F("    "));
  }
  else if (NumberMenu == 4){
    byte x = 0;
    lcd.setCursor(0,0);
    lcd.print(F("Error menu      "));
    lcd.setCursor(x,1);
    if(PostCode =! 0){
      lcd.print(F(""));
    }
    lcd.print("                ");
    if (ErrorCloseWindow_1 == true){
      lcd.setCursor(x,1);
      lcd.print(F("21"));
      x = x + 3;
    }
    if (ErrorCloseWindow_2 == true){
      lcd.setCursor(x,1);
      lcd.print(F("22"));
      x = x + 3;
    }
    if (ErrorCloseWindow_3 == true){
      lcd.setCursor(x,1);
      lcd.print(F("23"));
      x = x + 3;
    }
  }
} 

void WriteToLCD(String Text, byte line){
	lcd.backlight();
	#define DelayTime 1500
	switch (line){
		case 0:
			lcd.setCursor(0,0);
			lcd.print(Text);
			lcd.print("                ");
			delay(DelayTime);
			//lcd.clear();
		break;
			case 1:
			lcd.setCursor(0,1);
			lcd.print(Text);
			lcd.print("                ");
			delay(DelayTime);
			lcd.clear();
			break;
	}
}
