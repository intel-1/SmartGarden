#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include "lib\LiquidCrystal_I2C.h"
#include <stdint.h>
#include <Arduino.h>


extern LiquidCrystal_I2C lcd;



#define LCD_ICON_NOT_SIM 8

#define LCD_SCREEN_REFRESH_DELAY true
#define LCD_NO_SCREEN_REFRESH_DELAY false

#define LCD_ALLOW_OTPUT_ON_SCREEN true
#define LCD_NO_OTPUT_ON_SCREEN false

#define LCD_UPDATE_SCREEN 1
#define LCD_CONCLUSION_SCREEN 0

#define LCD_SCREEN_MAIN 1
#define LCD_SCREEN_MANAGEMENT 2
#define LCD_SCREEN_CONFIG 3


extern char buffer[16];
extern byte EnableKey;
extern boolean EncoderRead;
extern int StatusMenu;          // Текущее положение меню
extern boolean StatMenu;        // Вывод\обновления меню экрана. Значения 1\0 
extern boolean LightLCDEnable;  // Включена ли подсветка экрана. Значения 1\0 (включена\выключена)

void InitializingLCDicons();					// Инициализация значков для LCD экрана
void UpdateMenu();

void WriteToLCD(String Text, byte line, byte Position, bool Delay);
void ViewSignalLevel(byte Level);
void Clean_LCD(byte line, byte StartSymbol);

void WindowMenu(byte NumberMenu, byte Update); 

/*
 * Post коды ошибок:
   * 1 - ошибки датчиков: 
      * 11 - ReadTempOut 
      * 12 - ReadTempLeftUp
      * 13 - ReadTempLeftDown
      * 16 - датчик влажности воздуха offline
      * 17 - много ошибок чтения датчика влажности
      * 18 - ошибки влажности почвы левой грядки
      * 19 - ошибки влажности почвы правой грядки
   * 2 - ошибки работы окон: 
      * 21 - закрытия первого окна 
      * 22 - закрытия второго окна
      * 23 - закрытия третьего окна
   * 
   * 51 - Низкое напряжение VCC
   * 52 - Низкое напряжение Ext_VCC
 */

#endif
