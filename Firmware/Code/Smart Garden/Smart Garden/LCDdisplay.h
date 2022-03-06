#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include "lib\LiquidCrystal_I2C.h"
#include <stdint.h>
#include <Arduino.h>




extern LiquidCrystal_I2C lcd;

extern char buffer[16];
extern byte EnableKey;
extern boolean EncoderRead;
extern int StatusMenu;          // Текущее положение меню
extern boolean StatMenu;        // Вывод\обновления меню экрана. Значения 1\0 
extern boolean LightLCDEnable;  // Включена ли подсветка экрана. Значения 1\0 (включена\выключена)

void InitializingLCDicons();					// Инициализация значков для LCD экрана
void UpdateMenu();

void WriteToLCD(String Text, byte line);
void ViewSignalLevel(byte Level);

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
