#include <EEPROM.h>
#include "lib\iarduino_RTC.h"

#include "EEPROM_ADR.h"
#include "Warning.h"
#include "Sensors.h"
#include "main.h"
#include "LCDdisplay.h"
#include "GSM.h"

LiquidCrystal_I2C lcd(ADDRESS_INPUT_LCD,20,4);

#define LCD_Temp_Char 1
#define GSM_Signal_OFF 2
#define GSM_Signal_Marginal 3
#define GSM_Signal_OK 4
#define GSM_Signal_Good 5
#define GSM_Signal_Excellent 6

char buffer[16];
byte EnableKey = 1;
boolean EncoderRead = LOW;
int StatusMenu = 0;                 // Текущее положение меню
boolean StatMenu;                   // Вывод\обновления меню экрана. Значения 1\0
boolean LightLCDEnable = false;     // Включена ли подсветка экрана. Значения 1\0 (включена\выключена)


byte Icon_Signal_OFF[8] = {				// 99 dBm
	B11111,
	B00000,
	B01001,
	B00110,
	B01001,
	B00000,
	B10000 };
byte Icon_Signal_Marginal[8] = {			// 31 dBm
	B11111,
	B00000,
	B00000,
	B00000,
	B00000,
	B01000,
	B11000 };
byte Icon_Signal_OK[8] = {				// 2-30 dBm
	B11111,
	B00000,
	B00000,
	B00000,
	B00100,
	B01100,
	B11100 };
byte Icon_Signal_Good[8] = {				// 1 
	B11111,
	B00000,
	B00000,
	B00010,
	B00110,
	B01110,
	B11110 };
byte Icon_Signal_Excellent[8] = {		// 0
	B11111,
	B00000,
	B00001,
	B00011,
	B00111,
	B01111,
	B11111 };
	
byte Icon_TempChar[8] = {                // Значок градуса
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


void InitializingLCDicons(){								// Инициализация значков для LCD экрана
	lcd.createChar(LCD_Temp_Char, Icon_TempChar);			// Инициализация значка температуры
	lcd.createChar(GSM_Signal_OFF, Icon_Signal_OFF);
	lcd.createChar(GSM_Signal_Marginal, Icon_Signal_Marginal);
	lcd.createChar(GSM_Signal_OK, Icon_Signal_OK);
	lcd.createChar(GSM_Signal_Good, Icon_Signal_Good);
	lcd.createChar(GSM_Signal_Excellent, Icon_Signal_Excellent);
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
		lcd.print(char(GSM_Signal_Excellent));			// 	lcd.createChar(6, Signal_Excellent);
	}
	else if(_Level == 1){
		lcd.print(char(GSM_Signal_Good));				// 	lcd.createChar(5, Signal_Good);
	}
	else if(2 <= _Level <= 30){
		lcd.print(char(GSM_Signal_OK));					// 	lcd.createChar(4, Signal_OK);
	}
	else if(_Level == 31){
		lcd.print(char(GSM_Signal_Marginal));			// 	lcd.createChar(3, Signal_Marginal);
	}
	else if(_Level == 99){
		lcd.print(char(GSM_Signal_OFF));				// 	lcd.createChar(2, Signal_OFF);
	}
}


void Clean_LCD(byte line){
	lcd.setCursor(0,line);
	lcd.print(F("                    "));
}

void Output_Log_To_LCD(byte StartPosition, byte StartLine, String Text){
	static String BuferText[4];
	lcd.setCursor(StartPosition, StartLine);
	lcd.print(Short_VersionFirmware);
}


void WriteToLCD(String Text, byte line, byte Position, bool Delay){
	lcd.backlight();
// 	byte LongText = Text.length();
// 	String Space;
// 	for (byte i = 0; i <= 20 - LongText; i++) {
// 		Space = Space + " ";
// 	}		
	lcd.setCursor(Position,line);
	lcd.print(Text/* + Space*/);
	if(line == 0){
		lcd.setCursor(20,1);
		ViewSignalLevel(StateGSM.GSM_Signal_Level);				// Восстанавливаем значек уровня сигнала GSM сети
	}
	if(Delay){
		_delay_ms(1100);
	}
}


void WindowMenu(byte NumberMenu, byte Update){   
/*   
 *    StatMenu - номер меню
 *    Update - выводить статическую информацию или динамических данных (0\1)
 */
	StatusMenu = NumberMenu;
	lcd.setCursor(0,0);
	switch(NumberMenu){							  
		case 1:													// Первый экран 
			switch(Update){
				case 0:											// Выводить стартовую информацию
					lcd.clear();								// Очистка экрана
					if(EEPROM.read(E_ModeController) == 1){
						lcd.print(F("Automatic          "));	// "Automatic"
					}
					else{ 
						lcd.print(F("Manual             "));	// "Manual"
					}

					lcd.setCursor(LCD_START_SYMBOL_16,LCD_LINE_1);
					lcd.print(F("v"));
					
					
					lcd.setCursor(LCD_START_SYMBOL_1,LCD_LINE_2);
					lcd.print(F("Tint: "));
					lcd.setCursor(LCD_START_SYMBOL_11,LCD_LINE_2);
					lcd.print(char(LCD_Temp_Char));					// Значек градуса
					lcd.print(F("C"));								// "C"
					// ------------------------------------------
					lcd.setCursor(LCD_START_SYMBOL_1,LCD_LINE_3);
					lcd.print(F("Tup: "));
					lcd.setCursor(LCD_START_SYMBOL_10,LCD_LINE_3);
					lcd.print(char(LCD_Temp_Char));					// Значек градуса
					lcd.print(F("C"));								// "C"
					// ------------------------------------------
					lcd.setCursor(LCD_START_SYMBOL_1,LCD_LINE_4);
					lcd.print(F("Tdown: "));
					lcd.setCursor(LCD_START_SYMBOL_12,LCD_LINE_4);
					lcd.print(char(LCD_Temp_Char));					// Значек градуса
					lcd.print(F("C"));								// "C"
					// ------------------------------------------
					lcd.setCursor(LCD_START_SYMBOL_15,LCD_LINE_2);
					lcd.print(F("H:"));
					lcd.setCursor(LCD_START_SYMBOL_20,LCD_LINE_2);	
					lcd.print(F("%"));
					// ==============================================================					
					break;
				case 1:												// Обновление динамических данных
					lcd.setCursor(LCD_START_SYMBOL_12,LCD_LINE_1);
					lcd.print(VCC, 1);
					// ------------------------------------------
					lcd.setCursor(LCD_START_SYMBOL_7,LCD_LINE_2);
					lcd.print(Ti, 1);
					// ------------------------------------------
					lcd.setCursor(LCD_START_SYMBOL_6,LCD_LINE_3);
					lcd.print(RealValueSensors[SENSOR_6][VALUE_1], 1);
					// ------------------------------------------
					lcd.setCursor(LCD_START_SYMBOL_8,LCD_LINE_4);
					lcd.print(Ti, 1);
					// ------------------------------------------
					lcd.setCursor(LCD_START_SYMBOL_16,LCD_LINE_4);
					lcd.print(time.gettime("H:i"));
					// ------------------------------------------
					lcd.setCursor(LCD_START_SYMBOL_18,LCD_LINE_2);
					lcd.print(RealValueSensors[SENSOR_6][VALUE_2], 0);
					
					break;
			}
		case 2:
			lcd.setCursor(0,0);
			break;
		case 3:
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
			break;
		case 4:
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
			break;
	}
} 
