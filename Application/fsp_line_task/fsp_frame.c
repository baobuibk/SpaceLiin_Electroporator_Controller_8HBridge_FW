/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <string.h>
#include <stdlib.h>

#include "app.h"

#include "fsp_frame.h"
#include "crc.h"
//#include "pwm.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
//extern Accel_Gyro_DataTypedef _gyro, _accel;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void convertArrayToInteger(uint8_t arr[], uint16_t *p);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
extern uart_stdio_typedef *CMD_line_handle;

uint16_t Avr_Current;
uint16_t Impedance;
uint32_t Voltage;
uint16_t delay_ms = 0;
uint16_t FSP_lv_on_time_ms, FSP_lv_off_time_ms;

void FSP_Line_Process()
{
switch (ps_FSP_RX->CMD)
{   

case FSP_CMD_MEASURE_CURRENT:
{
    Avr_Current =   ps_FSP_RX->Payload.measure_current.Value_high;
    Avr_Current =   Avr_Current << 8;
    Avr_Current |=  ps_FSP_RX->Payload.measure_current.Value_low;

    if (Avr_Current < 1000)
    {
        UART_Printf(CMD_line_handle, "CURRENT IS %dmA\n", Avr_Current);
    }
    else
    {
        UART_Printf(CMD_line_handle, "CURRENT IS %d.%dA\n", Avr_Current / 1000, Avr_Current % 1000);
    }
    
    UART_Send_String(CMD_line_handle, "> ");
    break;
}
    

case FSP_CMD_MEASURE_IMPEDANCE:
{
    Voltage = g_Feedback_Voltage[0] * 1000 / hv_calib_coefficient.average_value;

    PID_is_300V_on = 0;
    PID_is_50V_on = 0;
    g_is_Discharge_300V_On = 1;
    g_is_Discharge_50V_On = 1;	

    Avr_Current =   ps_FSP_RX->Payload.measure_impedance.Value_high;
    Avr_Current =   Avr_Current << 8;
    Avr_Current |=  ps_FSP_RX->Payload.measure_impedance.Value_low;

    Impedance = Voltage / Avr_Current;

    UART_Send_String(CMD_line_handle, "MEASURING...OK\n");
    
    UART_Printf(CMD_line_handle, "> CURRENT IS %dmA\n", Avr_Current);

    UART_Printf(CMD_line_handle, "> IMPEDANCE IS %d Ohm\n", Impedance);

    UART_Send_String(CMD_line_handle, "> ");
    break;
}

case FSP_CMD_SENSOR_GET_TEMP:
{
    UART_Send_String(CMD_line_handle, "Received FSP_CMD_GET_TEMP\n");

    UART_Printf(CMD_line_handle, "> TEMPERATURE: %s Celsius\n", ps_FSP_RX->Payload.get_sensor_temp.temp);

    UART_Send_String(CMD_line_handle, "> ");
    break;
}

case FSP_CMD_SENSOR_GET_PRESSURE:
{
    UART_Send_String(CMD_line_handle, "Received FSP_CMD_GET_PRESSURE\n");

    UART_Printf(CMD_line_handle, "> PRESSURE: %s Pa\n", ps_FSP_RX->Payload.get_sensor_pressure.pressure);

    UART_Send_String(CMD_line_handle, "> ");
    break;
}

case FSP_CMD_SENSOR_GET_BMP390:
{
    UART_Send_String(CMD_line_handle, "Received FSP_CMD_GET_BMP390\n");

    UART_Printf(CMD_line_handle, "> TEMPERATURE: %s Celsius\n", ps_FSP_RX->Payload.get_sensor_BMP390.temp);

    UART_Printf(CMD_line_handle, "> PRESSURE: %s Pa\n", ps_FSP_RX->Payload.get_sensor_BMP390.pressure);

    UART_Send_String(CMD_line_handle, "> ");
    break;
}

case FSP_CMD_SENSOR_GET_ACCEL:
{
    UART_Send_String(CMD_line_handle, "Received FSP_CMD_GET_ACCEL\n");

    uint16_t accel_x=0, accel_y=0, accel_z=0;

    convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_accel.accel_x, &accel_x);
    convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_accel.accel_y, &accel_y);
    convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_accel.accel_z, &accel_z);

    UART_Printf(CMD_line_handle, "> ACCEL x: %dmm/s2; ACCEL y: %dmm/s2; ACCEL z: %dmm/s2\n", accel_x, accel_y, accel_z);

    UART_Send_String(CMD_line_handle, "> ");
    break;
}

case FSP_CMD_SENSOR_GET_GYRO:
{
    UART_Send_String(CMD_line_handle, "Received FSP_CMD_GET_GYRO\n");

    uint16_t gyro_x=0, gyro_y=0, gyro_z=0;

    convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_gyro.gyro_x,  &gyro_x);
    convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_gyro.gyro_y,  &gyro_y);
    convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_gyro.gyro_z,  &gyro_z);

    UART_Printf(CMD_line_handle, "> GYRO x: %dmpds; GYRO y: %dmpds; GYRO z: %dmpds\n", gyro_x, gyro_y, gyro_z);

    UART_Send_String(CMD_line_handle, "> ");
    break;
}

case FSP_CMD_SENSOR_GET_LSM6DSOX:
{
    UART_Send_String(CMD_line_handle, "Received FSP_CMD_GET_LSM6DSOX\n");

    uint16_t accel_x=0, accel_y=0, accel_z=0, gyro_x=0, gyro_y=0, gyro_z=0;

    convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_LSM6DSOX.accel_x, &accel_x);
    convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_LSM6DSOX.accel_y, &accel_y);
    convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_LSM6DSOX.accel_z, &accel_z);
    convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_LSM6DSOX.gyro_x,  &gyro_x);
    convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_LSM6DSOX.gyro_y,  &gyro_y);
    convertArrayToInteger(ps_FSP_RX->Payload.get_sensor_LSM6DSOX.gyro_z,  &gyro_z);

    UART_Printf(CMD_line_handle, "> ACCEL x: %dmm/s2; ACCEL y: %dmm/s2; ACCEL z: %dmm/s2\n", accel_x, accel_y, accel_z);

    UART_Printf(CMD_line_handle, "> GYRO x: %dmpds; GYRO y: %dmpds; GYRO z: %dmpds\n", gyro_x, gyro_y, gyro_z);

    UART_Send_String(CMD_line_handle, "> ");
    break;
}
    
default:
    break;
}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void convertArrayToInteger(uint8_t arr[], uint16_t *p)
{
	*p= (uint16_t)(arr[1]<<8 | arr[0]);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */