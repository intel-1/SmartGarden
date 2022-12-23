#include <./Arduino.h>
#include <./EEPROM.h>

#include "RecievedConfigModule.h"
#include "../EEPROM_ADR.h"
#include "../ParsingDataOnSerial.h"
#include "../Sensors.h"
#include "../DigitalPorts.h"
#include "../ExecModules.h"
#include "../main.h"



void SentConfigModuleUART(){
	Serial.print(F("======= Configuration module '")); Serial.print(InputFromSerial0[0]); Serial.println(F("' ======="));
	byte PortModule = EEPROM.read(E_PortExecModule + InputFromSerial0[0]);
	Serial.print(F("Состояние модуля: "/*"Status module: "*/));
	switch(EEPROM.read(E_StatusModule + InputFromSerial0[0])){
		case 0:
			Serial.println(F("Выключен"/*"off"*/));
			break;
		case 1:
			Serial.println(F("Включен"/*"on"*/));
			break;
		default:
			Serial.println(F("Ошибка"/*"Error data"*/));
	}
	Serial.print(F("Тип модуля: "/*"Type module: "*/));
	switch(EEPROM.read(E_TypeExecModule + InputFromSerial0[0])){		// Тип исполняемого модуля
		case 1:
			Serial.println(F("Шаговый мотор"/*"Stepper motor"*/));
			Serial.print(F("Порт включения драйвера: "/*"Control port: "*/));
			DigitalPort(PortModule, DIGITAL_PORT_OFF, DIGITAL_PORT_RETURN_NAME_PORT, LOG_TO_UART);
			Serial.println(F(""));
			Serial.print(F("Режим работы: "/*"Mode configure: "*/));
			switch(EEPROM.read(E_ManualModeModule + InputFromSerial0[0])){
				case 0:
					Serial.println(F("Автоматический"/*"Automatic"*/));
					break;
				case 1:
					Serial.println(F("Ручной"/*"Manual"*/));
					break;
			}
			Serial.print(F("Максимальная высота открытия: "/*"Max lift rotation: "*/)); Serial.print(EEPROM.read(E_MaxLimitRotation + InputFromSerial0[0])); Serial.println(F("см"));
			Serial.print(F("Максимальная скорость: "/*"setMaxSpeed: "*/)); Serial.print(EEPROM_int_read(E_StepperMotor_setMaxSpeed + InputFromSerial0[0] * 2)); Serial.println(F("шагов/секунду"));
			Serial.print(F("Максимальное ускорение: "/*"setAcceleration: "*/)); Serial.print(EEPROM_int_read(E_StepperMotor_setAcceleration + InputFromSerial0[0] * 2)); Serial.println(F("шагов/секунду^2"));
			Serial.print(F("Кол-во оборотов на 1см подъема: "/*"Step see/about: "*/)); Serial.print(EEPROM_int_read(E_StepperMotor_Step + InputFromSerial0[0] * 2)); Serial.println(F("шагов/см"));
			Serial.print(F("Делитель шага драйвера: ")); Serial.print(F("1/")); Serial.print(EEPROM.read(E_StepperMotor_DividerStep + InputFromSerial0[0])); Serial.println(F("шага"));
			break;   
		case 2:
			Serial.println(F("ШИМ"/*"PWM"*/));
			Serial.print(F("Управляющий порт"/*"Control port: "*/));
			if(AllowPWMport(PortModule)){	// Если порт управления в списке разрешенных для работы с ШИМ
				DigitalPort(PortModule, DIGITAL_PORT_OFF, DIGITAL_PORT_RETURN_NAME_PORT, NO_LOG_TO_UART);
				Serial.println(F(""));
			}
			else Serial.println(F("the port isn't configured"));
			Serial.print(F("Max lift rotation: ")); Serial.println(F("255"));
			break;
		case 3:
			Serial.println(F("Servo motor"));
			Serial.print(F("Управляющий порт: "));
			if(AllowServoPort(PortModule)){
				DigitalPort(PortModule, DIGITAL_PORT_OFF, DIGITAL_PORT_RETURN_NAME_PORT, NO_LOG_TO_UART);
				Serial.println(F(""));
			}
			else Serial.println(F("the port isn't configured"));
			Serial.print(F("Max angle of turn: ")); Serial.println(EEPROM.read(E_MaxLimitRotation + InputFromSerial0[0]));
			Serial.print(F("Mode configure: "));
			switch(EEPROM.read(E_ManualModeModule + InputFromSerial0[0])){
				case 0:
					Serial.println(F("Automatic"));
					break;
				case 1:
					Serial.println(F("Manual"));
					Serial.print(F("MinImp: ")); Serial.println(EEPROM.read(E_Servo_MinImp + InputFromSerial0[0]));
					Serial.print(F("MaxImp: ")); Serial.println(EEPROM.read(E_Servo_MaxImp + InputFromSerial0[0]));
					break;
			}
			break;
		case 4:
			Serial.println(F("Digital port"));
			Serial.print(F("Управляющий порт: "));
			DigitalPort(PortModule, DIGITAL_PORT_OFF, DIGITAL_PORT_RETURN_NAME_PORT, NO_LOG_TO_UART);
			Serial.println(F(""));
			break;
		default:
			Serial.println(F("The type of the module isn't configured"));
	}
	Serial.print(F("Группа: ")); Serial.println(EEPROM.read(E_BindExecModile + InputFromSerial0[0]));
	if(EEPROM.read(E_LowSwitchPortModule + InputFromSerial0[0]) != 0){
		Serial.print(F("Switch DOWN port: ")); ViewNameInputDigitalPorts(EEPROM.read(E_LowSwitchPortModule + InputFromSerial0[0])); Serial.println(F(""));
	}
	if(EEPROM.read(E_HighSwitchPortModule + InputFromSerial0[0]) != 0){
		Serial.print(F("Switch UP port: ")); ViewNameInputDigitalPorts(EEPROM.read(E_HighSwitchPortModule + InputFromSerial0[0])); Serial.println(F(""));
		Serial.print(F("Type UP switch: "));
		switch(EEPROM.read(E_TypeHighSwitchModule + InputFromSerial0[0])){
			case 1:
				Serial.println(F("Up switch"));
				break;
			case 2:
				Serial.println(F("Switch counter"));
				break;
			default:
				Serial.println("Error data");
		}
	}
	Serial.println(F("==================================="));
}


// ===============================================================================================================================
void SentConfigModuleExtApp(){
	Serial.print(InputFromSerial0[0]);																Serial.print(F(" "));	// Номер модуля
	Serial.print(F("0"));																			Serial.print(F(" "));	// Служебный байт
	Serial.print(EEPROM.read(E_StatusModule + InputFromSerial0[0]));								Serial.print(F(" "));	// Статус модуля (вкл\выкл)
	Serial.print(EEPROM.read(E_BindExecModile + InputFromSerial0[0]));								Serial.print(F(" "));	// Привязка модуля к группе
	Serial.print(EEPROM.read(E_TypeExecModule + InputFromSerial0[0]));								Serial.print(F(" "));	// Тип модуля
	Serial.print(EEPROM.read(E_MaxLimitRotation + InputFromSerial0[0]));							Serial.print(F(" "));	// Максимальная величина "открытия" модуля
	Serial.print(EEPROM.read(E_PortExecModule + InputFromSerial0[0]));								Serial.print(F(" "));	// Адрес управляющего порта
	Serial.print(EEPROM_int_read(E_StepperMotor_setMaxSpeed + InputFromSerial0[0] * 2));			Serial.print(F(" "));	// Максимальная скорость вращения (только для шагового мотора)
	Serial.print(EEPROM_int_read(E_StepperMotor_setAcceleration + InputFromSerial0[0] * 2));		Serial.print(F(" "));	// Ускорение мотора (только для шагового мотора)
	Serial.print(EEPROM_int_read(E_StepperMotor_Step + InputFromSerial0[0] * 2));					Serial.print(F(" "));	// Количество шагов на один см подъема
	Serial.print(EEPROM.read(E_StepperMotor_DividerStep + InputFromSerial0[0]));					Serial.print(F(" "));	// Делитель шага Stepper драйвера (Нужент для режима шагового мотора)
	Serial.print(EEPROM_int_read(E_Servo_MinImp + InputFromSerial0[0]*2));							Serial.print(F(" "));	// (тип int) Серво привод. Величина импульса для поворота в 0*
	Serial.print(EEPROM_int_read(E_Servo_MaxImp + InputFromSerial0[0]*2));							Serial.print(F(" "));	// (тип int) Серво привод. Величина импульса для поворота в 180*
	Serial.print(EEPROM.read(E_ManualModeModule + InputFromSerial0[0]));							Serial.print(F(" "));
	Serial.print(EEPROM.read(E_LowSwitchPortModule + InputFromSerial0[0]));							Serial.print(F(" "));	// Номер порта для концевика MIN
	Serial.print(EEPROM.read(E_HighSwitchPortModule + InputFromSerial0[0]));						Serial.print(F(" "));	// Номер порта для концевика MAX
	Serial.print(EEPROM.read(E_TypeHighSwitchModule + InputFromSerial0[0]));						Serial.print(F(" "));	// Тип концевика MAX
}



void WriteDataConfigExecModule(){
	if(InputFromSerial0[2] == 0 || InputFromSerial0[2] == 1){							// Разрешенное значение 0 или 1
		EEPROM.update(E_StatusModule + InputFromSerial0[0], InputFromSerial0[2]);		// Статус модуля (вкл\выкл)
	}
	// ---------------- Привязка модуля к группе ----------------
	EEPROM.update(E_BindExecModile + InputFromSerial0[0], InputFromSerial0[3]);			// Привязка модуля к группе
	// ---------------- Тип модуля ----------------
	if(1 <= InputFromSerial0[4] && InputFromSerial0[4] <= 6){							// Разрешенное значение 1 - 6
		EEPROM.update(E_TypeExecModule + InputFromSerial0[0], InputFromSerial0[4]);		// Тип модуля		
	}
	// ---------------- Максимальная величина открытия модуля ----------------
	EEPROM.update(E_MaxLimitRotation + InputFromSerial0[0], InputFromSerial0[5]);					// Максимальная величина открытия модуля
	// ---------------- Порт управления модулем ----------------
	for(byte Module = 1; Module <= QUANTITY_EXEC_MODULES; Module ++){								// ищем во всех байтах конфигурации данные совпадающие с введенным из UART значением (защита от дурака)
		if(EEPROM.read(E_PortExecModule + Module) == InputFromSerial0[5]){							// и если находим
			EEPROM.write(E_PortExecModule + Module, 0);												// то затираем ее нулями чтобы данные датчика не привязались двум разным группам
		}
	}
	if(EEPROM.read(E_PortExecModule + InputFromSerial0[0]) != InputFromSerial0[6]){					// Если меняется номер управляющего порта
		DigitalPort(InputFromSerial0[6], StateDigitalPorts[InputFromSerial0[6] - 1], DIGITAL_PORT_SWITCH_PORT, NO_LOG_TO_UART);			// то копируем состояние старого порта в новый
		DigitalPort(EEPROM.read(E_PortExecModule + InputFromSerial0[0]), DIGITAL_PORT_OFF, DIGITAL_PORT_SWITCH_PORT, NO_LOG_TO_UART);	// Выключаем старый порт
		StateDigitalPorts[InputFromSerial0[6] - 1] = 0;												// Обнуляем состояние старого порта в массиве
		EEPROM.write(E_PortExecModule + InputFromSerial0[0], InputFromSerial0[6]);					// Записываем адрес порта управления модулем
	}
	// ---------------- Максимальная скорость вращения шаговика ----------------
	EEPROM.put(E_StepperMotor_setMaxSpeed + InputFromSerial0[0]*2, InputFromSerial0[7]);			// (тип int) Максимальная скорость вращения ротора двигателя (шагов/секунду)
	// ---------------- Максимальное ускорение шаговика ----------------
	EEPROM.put(E_StepperMotor_setAcceleration + InputFromSerial0[0]*2, InputFromSerial0[8]);		// (тип int) Ускорение мотора (шагов/секунду^2)
	// ---------------- Кол-во шагов шаговика на один см подъема ----------------
	if(EEPROM_int_read(E_StepperMotor_Step + InputFromSerial0[0]*2) != InputFromSerial0[9]){		// Если изменяем кол-во шагов на оборот шпильки
		EEPROM.put(E_StepperMotor_Step + InputFromSerial0[0]*2, InputFromSerial0[9]);				// (тип int) Кол-во оборотов на 1мм шпильки
		//InitializingExecModule(InputFromSerial_config_module[0]);									// Заного инициализируем шаговый мотор, т.е. перемещаем его в нули
	}
	// ---------------------- Серво мотор. Делитель шага драйвера ----------------------
	if(InputFromSerial0[10] == 1 ||	InputFromSerial0[10] == 2 || InputFromSerial0[10] == 4 || InputFromSerial0[10] == 8 || InputFromSerial0[10] == 16 || InputFromSerial0[10] == 32 || InputFromSerial0[10] == 64){
		EEPROM.update(E_StepperMotor_DividerStep + InputFromSerial0[0], InputFromSerial0[10]);
	}
	else EEPROM.update(E_StepperMotor_DividerStep + InputFromSerial0[0], 1);						// default значение если вводим не правильные данные
	// ---------------- Серво мотор. Величина импульса для поворота в 0" ----------------
	EEPROM.put(E_Servo_MinImp + InputFromSerial0[0]*2, InputFromSerial0[11]);						// (тип int) Серво привод. Величина импульса для поворота в 0*
	// ---------------- Серво мотор. Величина импульса для поворота в 180" ----------------
	EEPROM.put(E_Servo_MaxImp + InputFromSerial0[0]*2, InputFromSerial0[12]);						// (тип int) Серво привод. Величина импульса для поворота в 180*
	// ---------------- Ручной\автоматический режим работы мотора ----------------
	if(InputFromSerial0[13] == 0 || InputFromSerial0[13] == 1){										// Можно вводить только 1 или 2
		EEPROM.update(E_ManualModeModule + InputFromSerial0[0], InputFromSerial0[13]);				// Стандартный или ручной режим конфигурирования модуля
	}
	// ---------------- Порты концевиков ----------------
	if(InputFromSerial0[14] == InputFromSerial0[15]){												// Если ввели два одинаковых значения
		InputFromSerial0[15] = 0;
	}
	if(PORT_INPUT_D_PIN_1 <= InputFromSerial0[14] && InputFromSerial0[14] <= PORT_INPUT_D_PIN_9){	// Разрешенные значения с 41-го по 49-й
		CleaningDuplicatedPorts(InputFromSerial0[14]);												// Защита чтобы один и тот же порт не был добавлен разным модулям
		EEPROM.update(E_LowSwitchPortModule + InputFromSerial0[0], InputFromSerial0[14]);			// Адреса портов для концевиков модулей
	}
	if(PORT_INPUT_D_PIN_1 <= InputFromSerial0[14] && InputFromSerial0[15] <= PORT_INPUT_D_PIN_9){	// Разрешенные значения с 41-го по 49-й	
		CleaningDuplicatedPorts(InputFromSerial0[15]);												// Защита чтобы один и тот же порт не был добавлен разным модулям
		EEPROM.update(E_HighSwitchPortModule + InputFromSerial0[0], InputFromSerial0[15]);			// Адреса портов для концевиков модулей
	}
	// ---------------- Тип HIGH концевика ----------------
	if(1 <= InputFromSerial0[16] && InputFromSerial0[16] <= 2){										// Разрешенные значения 1 и 2
		EEPROM.update(E_TypeHighSwitchModule + InputFromSerial0[0], InputFromSerial0[16]);
	}
	
	Serial.println();
	Serial.print(F("Settings of the module '")); Serial.print(InputFromSerial0[0]); Serial.println(F("' is updated"));
}


// ===============================================================================================================================
void RecievedConfigExecModule(){
	boolean flag;
	for (byte i = 2; i < sizeof(InputFromSerial0)/sizeof(int); i++){
		if (InputFromSerial0[i] == 0){
			flag = false;
		}
		else{
			flag = true;
			break;
		}
	}
	
	if(1 <= InputFromSerial0[0] && InputFromSerial0[0] <= QUANTITY_EXEC_MODULES || InputFromSerial0[0] == 255){
		if (flag){
			if(InputFromSerial0[0] != 255){
				WriteDataConfigExecModule();				// Сохраняем конфигурацию
			}
		}
		else{
			switch (InputFromSerial0[1]){
				case 1:
					if(InputFromSerial0[0] != 255){
						Serial.print(F("m "));
						SentConfigModuleExtApp();
						Serial.println();
					}
					else{												// Выводим полную инфу для отладки
						for(byte Module = 1; Module <= QUANTITY_EXEC_MODULES; Module ++){
							Serial.print(F("m "));
							InputFromSerial0[0] = Module;
							SentConfigModuleExtApp();
						}
						Serial.println();
					}
					break;
				case 0:													// Выводим только байты конфигурации
					if(InputFromSerial0[0] != 255){
						SentConfigModuleUART();
					}
					else{												// Выводим полную инфу для отладки
						for(byte Module = 1; Module <= QUANTITY_EXEC_MODULES; Module ++){
							InputFromSerial0[0] = Module;
							SentConfigModuleUART();
						}
					}
					break;
			}
		}
	}
	CleanInputFromSerial0();
	recievedFlag_config_module = false;
	flag = false;
}


