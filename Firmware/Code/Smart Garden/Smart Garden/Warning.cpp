
#include <Arduino.h>
#include <EEPROM.h>
#include <stdint.h>

#include "Warning.h"
#include "EEPROM_ADR.h"
#include "lib\EPSKlibrary.h"


boolean GlobalError = false;            // Глобальная ошибка, работать дальше не возможно!!!!

boolean ErrorCloseWindow_1 = false;     // Ошибка закрытия первого окна
boolean ErrorCloseWindow_2 = false;     // Ошибка закрытия второго окна
boolean ErrorCloseWindow_3 = false;     // Ошибка закрытия третьего окна
byte NumberStepForCloseWindow = 30;     // Число мм для закрытия окна когда температура меньше минимальной (3 см)

boolean ErrorSDcard = false;            // Ошибка работы с SD картой

boolean HumidityAirOffline = false;     // Датчик влажность Offline

boolean ErrorTempLeft = false;			// Ни одна температура левой теплицы не известна
boolean ErrorTempRight = false;			// Ни одна температура правой теплицы не известна
boolean ErrorTempLeftUp = false;
boolean ErrorTempLeftDown = false;
boolean ErrorTempBox = false;

//---------- Ошибки напряжения питания ---------------
boolean Low_Input_VCC = false;				// Напряжение ниже минимального

//----------- Warning рабочих температур --------------
boolean WarningMaxTempLeft = false;		// Превышена максимальная температура левой теплицы
boolean WarningMaxTempRight = false;    // Превышена максимальная температура правой теплицы
boolean WarningMinTempLeft = false;
boolean WarningMinTempRight = false;

unsigned long LoopFrequencyLight5 = millis();
unsigned long LoopFrequencyLight6 = millis();

byte PostCode = 0;

void StatusLED(byte Warning){
/*
Функция сигнальных светодиодов.
Описание:
  0 - загрузка прошла штатно ()
  1 - низкое напряжение питания
  2 - ошибки SD карты
    21 - отсутствует SD карта 
    22 - ошибка записи на SD карту
  3 - GSM модуля:
    31 - отстутвует GSM модуля
    32 - нет регистрации в сети
    33 - низкий баланс счета
  4 - низкий заряд батареи
  5 - ошибка считывания датчиков:
    51 - влажности почвы
    52 - влажности воздуха
    53 - температуры внутри
    54 - температуры снаружи
  6 - ошибки окон
 */
	#define delay_blink 300
	status_led_all_off();
	switch(Warning){
		case 0:
			for(byte i=0; i<=2; i++){
				status_led_green_on();
				_delay_ms(delay_blink);
				status_led_green_off();
				status_led_blue_on();
				_delay_ms(delay_blink);
				status_led_blue_off();
				status_led_red_on();
				_delay_ms(delay_blink);
				status_led_red_off();
				_delay_ms(delay_blink);
			}
			for(byte i=0; i<=4; i++){
				status_led_green_invert();
				status_led_red_invert();
				status_led_blue_invert();		
				_delay_ms(delay_blink);
			}    
			break;
		case 1:
			for(byte i=0; i<=2; i++){
				status_led_red_on();
				_delay_ms(400);
				status_led_red_off();
				_delay_ms(400);
				
				status_led_red_on();
				_delay_ms(400);
				status_led_red_off();
			}
			break;
		case 21:
			for(byte i = 0; i<=3; i++){
				status_led_blue_on();
				status_led_green_on();
				_delay_ms(delay_blink);
				status_led_blue_off();
				status_led_green_off();
				_delay_ms(delay_blink);
			}
			break;  
		case 22:
			for(byte i = 0; i<=3; i++){
				status_led_blue_on();
				_delay_ms(delay_blink);
				status_led_blue_off();
				_delay_ms(delay_blink);
			}
			break;
		case 2:
			for (byte i = 0; i<=3; i++){        
				status_led_red_on();
				_delay_ms(delay_blink);
				status_led_red_off();
				_delay_ms(delay_blink);
			}  
			break;
		case 5:
			for (byte i = 0; i<=3; i++){ 
				status_led_green_on();
				_delay_ms(delay_blink);
				status_led_green_off();
				_delay_ms(delay_blink);
			}
			break;
		case 6:
			for (byte i = 0; i<=3; i++){ 
				status_led_red_on();
				_delay_ms(delay_blink);
				status_led_red_off();
				_delay_ms(delay_blink);
			}
			break;
	}
	status_led_all_off();
}