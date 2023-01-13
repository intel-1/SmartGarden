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
#define GSM_Signal_Very_Good 6
#define GSM_Signal_Excellent 7
#define LCD_ICON_NOT_SIM 8


char buffer[16];
byte EnableKey = 1;
boolean EncoderRead = LOW;
int StatusMenu = 0;                 // Текущее положение меню
boolean StatMenu;                   // Вывод\обновления меню экрана. Значения 1\0
boolean LightLCDEnable = false;     // Включена ли подсветка экрана. Значения 1\0 (включена\выключена)


byte Icon_Not_SIM[8] = {				// Нет SIM карты
	B01111,
	B10001,
	B11011,
	B10101,
	B11011,
	B10001,
	B11111 };
byte Icon_Signal_OFF[8] = {				// 99 dBm
	B11111,
	B00000,
	B01001,
	B00110,
	B01001,
	B00000,
	B10000 };
byte Icon_Signal_Marginal[8] = {		// 31 dBm
	B11111,
	B00000,
	B00000,
	B00000,
	B00000,
	B00000,
	B10000 };
byte Icon_Signal_OK[8] = {				// 2-30 dBm
	B11111,
	B00000,
	B00000,
	B00000,
	B00000,
	B01000,
	B11000 };
byte Icon_Signal_Good[8] = {			// 1
	B11111,
	B00000,
	B00000,
	B00000,
	B00100,
	B01100,
	B11100 };
byte Icon_Signal_Very_Good[8] = {			// 1 
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


void InitializingLCDicons(){										// Инициализация значков для LCD экрана
	lcd.createChar(LCD_Temp_Char, Icon_TempChar);					// Инициализация значка температуры
	lcd.createChar(GSM_Signal_OFF, Icon_Signal_OFF);
	lcd.createChar(GSM_Signal_Marginal, Icon_Signal_Marginal);
	lcd.createChar(GSM_Signal_OK, Icon_Signal_OK);
	lcd.createChar(GSM_Signal_Good, Icon_Signal_Good);
	lcd.createChar(GSM_Signal_Very_Good, Icon_Signal_Very_Good);
	lcd.createChar(GSM_Signal_Excellent, Icon_Signal_Excellent);
	lcd.createChar(LCD_ICON_NOT_SIM, Icon_Not_SIM);
}


void UpdateMenu(){
	switch (StatusMenu){
		case 1:
			WindowMenu(1, LCD_UPDATE_SCREEN);
			break;
	}		
}


void ViewSignalLevel(byte _Level){
	lcd.setCursor(LCD_START_SYMBOL_20, LCD_LINE_1);
	if (_Level == 99){
		lcd.print(char(GSM_Signal_OFF));
	}
	if (_Level == 52){
		lcd.print(char(GSM_Signal_Excellent));
	}
	if (54 <= _Level && _Level <= 81){
		lcd.print(char(GSM_Signal_Very_Good));
	}
	if (82 <= _Level && _Level <= 110){
		lcd.print(char(GSM_Signal_Good));
	}
	if (_Level == 111){
		lcd.print(char(GSM_Signal_OK));
	}	
	if (_Level == 115){
		lcd.print(char(GSM_Signal_Marginal));
	}	
}


void Clean_LCD(byte line, byte StartSymbol){
	String Space = "";
	lcd.setCursor(0,line);
	for(byte i = StartSymbol; i <= LCD_START_SYMBOL_20; i++){
		Space += " ";	
	}
	lcd.print(Space);
}


void Output_Log_To_LCD(byte StartPosition, byte StartLine, String Text){
	static String BuferText[4];
	lcd.setCursor(StartPosition, StartLine);
	lcd.print(Short_VersionFirmware);
}


void WriteToLCD(String Text, byte line, byte Position, bool Delay){
	lcd.backlight();
	lcd.setCursor(Position,line);
	lcd.print(Text/* + Space*/);
	if(Delay){
		_delay_ms(1100);
	}
}

void WindowMenu(byte NumberScreen, byte Update){   
/*   
 *    NumberScreen - номер экрана
 *    Update - выводить статическую информацию или динамических данных (0\1)
 */
	StatusMenu = NumberScreen;
	lcd.setCursor(LCD_START_SYMBOL_1,LCD_LINE_1);
	switch(NumberScreen){							  
		case LCD_SCREEN_MAIN:									// Первый экран 
			switch(Update){
				case LCD_CONCLUSION_SCREEN:						// Выводить стартовую информацию
					lcd.clear();								// Очистка экрана
					if(EEPROM.read(E_ModeController) == 1){
						lcd.print(F("Automatic          "));	// "Automatic"
					}
					else{ 
						lcd.print(F("Manual             "));	// "Manual"
					}

					lcd.setCursor(LCD_START_SYMBOL_17,LCD_LINE_1);
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
				case LCD_UPDATE_SCREEN:								// Обновление динамических данных
					lcd.setCursor(LCD_START_SYMBOL_12,LCD_LINE_1);
					lcd.print(VCC, 2);
					// ------------------------------------------
					lcd.setCursor(LCD_START_SYMBOL_7,LCD_LINE_2);
					lcd.print(Ti, 1);
					// ------------------------------------------
					lcd.setCursor(LCD_START_SYMBOL_6,LCD_LINE_3);
					lcd.print(Sensors.PresentValue[SENSOR_1][VALUE_1], 1);
					// ------------------------------------------
					lcd.setCursor(LCD_START_SYMBOL_8,LCD_LINE_4);
					lcd.print(Sensors.PresentValue[SENSOR_5][VALUE_1], 1);
					// ------------------------------------------
					lcd.setCursor(LCD_START_SYMBOL_16,LCD_LINE_4);
					lcd.print(time.gettime("H:i"));
					// ------------------------------------------
					lcd.setCursor(LCD_START_SYMBOL_18,LCD_LINE_2);
					lcd.print(Sensors.PresentValue[SENSOR_6][VALUE_2], 0);
					
					break;
			}
		case LCD_SCREEN_MANAGEMENT:
			lcd.setCursor(0,0);
			break;
		case LCD_SCREEN_CONFIG:
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
	}
} 
