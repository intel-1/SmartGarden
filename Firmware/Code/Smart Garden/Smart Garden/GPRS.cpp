#include <Arduino.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/wdt.h>


#include "GSM.h"
#include "GPRS.h"


// debug

// ========================================================================================================
// ============================= �������� ������ �� Web ������� �� GET ������ =============================
// ========================================================================================================
void Answer_check_GET(String Text, bool LogView){
	
	StateGSM.Code_Error_Sent_GET = 255;				// default ������ "100"
			
	if(LOGING_TO_SERIAL == UART_LOG_LEVEL_GSM || LOGING_TO_SERIAL == UART_LOG_LEVEL_ALL || ControllerSetup){
		if(LogView){
			Serial.print(F("Response from WEB server: "));
		}
	}

	String Response[] = {	(F("404")),				// �������� �� �������
							(F("200")),				// GET ������ ������� �������!!!
							(F("603"))};			// ������ �� ��������
	
	for(byte Pos = 0; Pos < sizeof(Response) / sizeof(Response[0]); Pos ++){						// ���� � ������ ��������� �������
		if(Text.lastIndexOf(Response[Pos]) != -1){				// � ���� ����� ����������
			if(OUTPUT_LEVEL_UART_GSM){
				switch(Pos){
					case 0:
						StateGSM.Code_Error_Sent_GET = 404;
						if(LogView){
							Serial.println(F("Page not found"));
						}
						break;
					case 1:
						StateGSM.Code_Error_Sent_GET = 200;
						if(LogView){
							Serial.println(F("OK"));
						}
						break;
					case 2:
						StateGSM.Code_Error_Sent_GET = 603;
						if(LogView){
							Serial.println(F("Server not available"));
						}
						break;
					default:
						if(LogView){
							Serial.println();
						}
					}
			}
		}
	}
}


// ======================================================================================================
// ======================================================================================================
// ======================================================================================================
void ViewErrorConnectGPRS(byte ErrorConnectGPRS, byte lang){
	switch(ErrorConnectGPRS){
		case 0:
			Serial.print(F("The connection is established"/*"���������� ���������������"*/));
			break;
		case 1:
			Serial.print(F("GPRS is connected"/*"GPRS ���������"*/));
			break;
		case 2:
			Serial.print(F("The connection is closed"/*"���������� �����������"*/));
			break;
		case 3:
			Serial.print(F("No connection"/*"��� ����������"*/));
			break;
		case 4:
			Serial.print(F("No data from GSM module"/*"��� ������ �� GSM ������"*/));
			break;
	}
}


// ======================================================================================================
// ========================================= ������������� GPRS =========================================
// ======================================================================================================
void InitializingGPRS(){
	String GPRS_ATs[] = {										// ������ �� ������ ������������� GPRS
		(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"")),				// ��������� �������� �����������
		(F("AT+SAPBR=3,1,\"APN\",\"internet.beeline.ru\"")),	// ��� APN ����� �������
		(F("AT+SAPBR=3,1,\"USER\",\"beeline\"")),				// ��� ������������
		(F("AT+SAPBR=3,1,\"PWD\",\"beeline\"")),				// ������
		(F("AT+SAPBR=1,1")),									// ��������� GPRS ����������
		(F("AT+HTTPINIT")),										// ������������� http �������
	};
	String Word;
	if(OUTPUT_LEVEL_UART_GSM){
		Serial.println(F("============= Start GPRS ============="));
	}
	for (byte i = 0; i < sizeof(GPRS_ATs) / sizeof(GPRS_ATs[0]); i++) {
		wdt_reset();
		Word = sendATCommand(GPRS_ATs[i], YES, YES);			// ���������� �� �������, ����, �������� � ������� ����� � Serial
		byte TimerCommand = 1;
		while(TimerCommand <= 4){								// ������������ ���-�� �������� �������
			if(Word.lastIndexOf("OK") != -1){					// ���� ����� "OK"
				TimerCommand = 5;								// ������������� ���������� ����� while
				goto end_while;
			}
			else{
				Word = sendATCommand(GPRS_ATs[i], YES, NO);	// �������� ���������� �������
				//Serial.println("WAIT");
				TimerCommand ++;
			}
		}
		end_while: ;
	}
	ConnectionGPRS();											// ��������� �����������
}


// ======================================================================================================
// ===================================== �������� ����������� GPRS ======================================
// ======================================================================================================
bool ConnectionGPRS(){
	StateGSM.GPRS_Connect = false;				// ���������� ���� �����������
	StateGSM.IP_GPRS = "0";
	
	if(OUTPUT_LEVEL_UART_GSM){
		Serial.print(F("State GPRS Connection: "));
	}
	String Response[] = {	(F("1,0")),			// ���������� ���������������
							(F("1,1")),			// ���������� �����������
							(F("1,2")),			// ���������� �����������
							(F("1,3"))};		// ��� ����������
	
	String Error = StateGSM.IP_GPRS = sendATCommand(F("AT+SAPBR=2,1"), true, false);	// ������ ��������� ����������
	
	if(Error.lastIndexOf(F("+SAPBR")) != -1){					// ��������� ������������ ������, ��� �� ������ �� ��� ������
		
		// IP �����
		StateGSM.IP_GPRS.remove(1,13);
		StateGSM.IP_GPRS.remove(StateGSM.IP_GPRS.length()-4);
		StateGSM.IP_GPRS.trim();
		StateGSM.IP_GPRS.remove(StateGSM.IP_GPRS.length()-1);
		//--//--//--//
	
		for(byte Pos = 0; Pos <= sizeof(Response) / sizeof(Response[0]); Pos ++){	// ���� � ������ ��������� �������
			if(Error.lastIndexOf(Response[Pos]) != -1){								// � ���� ����� ����������
				StateGSM.Code_Connect_GPRS = Pos;									// ��������� ��� GPRS ����������
				switch(Pos){
					case 1:															// ���� ���������� ����������� (����� "+SAPBR: 1,1")
						if(OUTPUT_LEVEL_UART_GSM){
							Serial.print(F("Established ("));
							Serial.print(StateGSM.IP_GPRS);
							Serial.println(F(")"));
							Serial.println(F("=============================================="));
						}
						StateGSM.GPRS_Connect = true;		// ��������� ����. ������� ��� ��������
						break;
				}
				Pos = 4;									// ����������� ������� ����� ���������� ���������� �����
			}
			if(Pos == 3){									// ���� ��������� ��� ������ � �� ����� ����������, �� ������� ��� ������ ������ �� �������
				StateGSM.Code_Connect_GPRS = 4;				// ��������� ������ "4"
			}
		}
	}
	return StateGSM.GPRS_Connect;
}


// =====================================================================================================
// ======================================= �������� GET ������� ========================================
// =====================================================================================================
void SendGETGPRS(String Text){
	wdt_reset();
	if(EEPROM.read(E_AllowGPRS) == ON){								// ���� ��������� ������ GPRS
		if(OUTPUT_LEVEL_UART_GSM){
			Serial.print(F("Sending a GET request: "));
		}
		sendATCommand(F("AT+HTTPPARA=\"CID\",1"), true, false);		// ��������� CID ��������� ��� http ������
		String Answer = sendATCommand(Text, true, false);			// ���� ��� ������
		if(Answer.lastIndexOf(F("OK")) != -1){						// ���� ������ ������� ���������	
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println(F("OK"));
			}
			// ========================= ��������� ����� �� ������� ==========================================
			String val = waitResponse(ON);					// ���� ����� �� Web �������
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println(val);
			}
			if (val.indexOf(F("+HTTPACTION")) > -1) {		// ����� �� GET ������
				Answer_check_GET(val, ON);					// ��������� ����� � ������� ��� � Serial
			}
		}
		else{
			StateGSM.Error_Sent_GET = true;					// ��������� ����, ��� GET �� ���������
			if(OUTPUT_LEVEL_UART_GSM){
				Serial.println(F("ERROR"));
			}
		}
		sendATCommand(F("AT+HTTPACTION=0"), true, false);	// �������� http ������		
	}
}
