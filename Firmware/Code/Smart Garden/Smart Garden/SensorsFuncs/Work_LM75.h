
#ifndef WORK_LM75_H
#define WORK_LM75_H



#define LM75_SHUTDOWN_NORMAL 0
#define LM75_SHUTDOWN_SHUTDOWN 1

#define LM75_TEMP_REGISTER 0
#define LM75_CONF_REGISTER 1
#define LM75_THYST_REGISTER 2
#define LM75_TOS_REGISTER 3

#define LM75_CONF_SHUTDOWN  0
#define LM75_CONF_OS_COMP_INT 1
#define LM75_CONF_OS_POL 2
#define LM75_CONF_OS_F_QUE 3



float LM75_temp (byte Address);
byte LM75_Read_Conf_OS (byte Address);
void LM75_Write_Conf_OS (byte data, byte Address);
float LM75_Read_tos_byte (byte Address);
void LM75_Write_tos_byte (float temp, byte Address);
float LM75_Read_thyst_byte (byte Address);
void LM75_Write_thyst_byte (float temp, byte Address);

#endif