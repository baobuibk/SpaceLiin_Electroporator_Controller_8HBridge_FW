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
static void convertArrayToInteger(uint8_t arr[], uint32_t *p);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

uint16_t Avr_Current;
uint16_t Impedance;
uint32_t Voltage;
uint16_t delay_ms = 0;
uint16_t lv_on_time_ms, lv_off_time_ms;
void FSP_Line_Process()
{
switch (pu_GPP_FSP_Payload->commonFrame.Cmd)
{
case FSP_CMD_SEND_PULSE_COUNT:
{
    UART_Printf(&RS232_UART, "POS HV PULSE COUNT: %d; NEG HV PULSE COUNT: %d\n", 
    pu_GPP_FSP_Payload->send_pulse_count.HV_pos_count, pu_GPP_FSP_Payload->send_pulse_count.HV_neg_count);
    UART_Printf(&RF_UART, "POS HV PULSE COUNT: %d; NEG HV PULSE COUNT: %d\n", 
    pu_GPP_FSP_Payload->send_pulse_count.HV_pos_count, pu_GPP_FSP_Payload->send_pulse_count.HV_neg_count);

    UART_Printf(&RS232_UART, "> POS LV PULSE COUNT: %d; NEG LV PULSE COUNT: %d\n", 
    pu_GPP_FSP_Payload->send_pulse_count.LV_pos_count, pu_GPP_FSP_Payload->send_pulse_count.LV_neg_count);
    UART_Printf(&RF_UART, "> POS LV PULSE COUNT: %d; NEG LV PULSE COUNT: %d\n", 
    pu_GPP_FSP_Payload->send_pulse_count.LV_pos_count, pu_GPP_FSP_Payload->send_pulse_count.LV_neg_count);

    UART_Send_String(&RS232_UART, "> ");
    UART_Send_String(&RF_UART, "> ");
    break;
}
    
case FSP_CMD_SEND_PULSE_DELAY:
{
    UART_Printf(&RS232_UART, "DELAY BETWEEN HV POS AND NEG PULSE: %dms\n", 
    pu_GPP_FSP_Payload->send_pulse_delay.HV_delay);
    UART_Printf(&RF_UART, "DELAY BETWEEN HV POS AND NEG PULSE: %dms\n", 
    pu_GPP_FSP_Payload->send_pulse_delay.HV_delay);

    UART_Printf(&RS232_UART, "> DELAY BETWEEN LV POS AND NEG PULSE: %dms\n", 
    pu_GPP_FSP_Payload->send_pulse_delay.LV_delay);
    UART_Printf(&RF_UART, "> DELAY BETWEEN LV POS AND NEG PULSE: %dms\n", 
    pu_GPP_FSP_Payload->send_pulse_delay.LV_delay);

    delay_ms = pu_GPP_FSP_Payload->send_pulse_delay.Delay_high;
    delay_ms <<= 8;
    delay_ms |= pu_GPP_FSP_Payload->send_pulse_delay.Delay_low;
    UART_Printf(&RS232_UART, "> DELAY BETWEEN HV PULSE AND LV PULSE: %dms\n", delay_ms);
    UART_Printf(&RF_UART, "> DELAY BETWEEN HV PULSE AND LV PULSE: %dms\n", delay_ms);

    UART_Send_String(&RS232_UART, "> ");
    UART_Send_String(&RF_UART, "> ");
    break;
}
    
case FSP_CMD_SEND_PULSE_HV:
{
    UART_Printf(&RS232_UART, "HV PULSE ON TIME: %dms; HV PULSE OFF TIME: %dms\n", 
    pu_GPP_FSP_Payload->send_pulse_HV.OnTime, pu_GPP_FSP_Payload->send_pulse_HV.OffTime);
    UART_Printf(&RF_UART, "HV PULSE ON TIME: %dms; HV PULSE OFF TIME: %dms\n", 
    pu_GPP_FSP_Payload->send_pulse_HV.OnTime, pu_GPP_FSP_Payload->send_pulse_HV.OffTime);

    UART_Send_String(&RS232_UART, "> ");
    UART_Send_String(&RF_UART, "> ");
    break;
}
    
case FSP_CMD_SEND_PULSE_LV:
{
    lv_on_time_ms = pu_GPP_FSP_Payload->send_pulse_LV.OnTime_high;
    lv_on_time_ms <<= 8;
    lv_on_time_ms |= pu_GPP_FSP_Payload->send_pulse_LV.OnTime_low;

    lv_off_time_ms = pu_GPP_FSP_Payload->send_pulse_LV.OffTime_high;
    lv_off_time_ms <<= 8;
    lv_off_time_ms |= pu_GPP_FSP_Payload->send_pulse_LV.OffTime_low;

    UART_Printf(&RS232_UART, "LV PULSE ON TIME: %dms; LV PULSE OFF TIME: %dms\n", lv_on_time_ms, lv_off_time_ms);
    UART_Printf(&RF_UART, "LV PULSE ON TIME: %dms; LV PULSE OFF TIME: %dms\n", lv_on_time_ms, lv_off_time_ms);

    UART_Send_String(&RS232_UART, "> ");
    UART_Send_String(&RF_UART, "> ");
    break;
}
    
case FSP_CMD_SEND_PULSE_CONTROL:
{
    if (pu_GPP_FSP_Payload->send_pulse_control.State == 1)
    {
        UART_Send_String(&RS232_UART, "H BRIDGE IS PULSING\n");
        UART_Send_String(&RF_UART, "H BRIDGE IS PULSING\n");
    }
    else
    {
        UART_Send_String(&RS232_UART, "H BRIDGE IS NOT PULSING\n");
        UART_Send_String(&RF_UART, "H BRIDGE IS NOT PULSING\n");
    }

    UART_Send_String(&RS232_UART, "> ");
    UART_Send_String(&RF_UART, "> ");
    break;
}
    
case FSP_CMD_SEND_PULSE_ALL:
{
    UART_Printf(&RS232_UART, "POS HV PULSE COUNT: %d; NEG HV PULSE COUNT: %d\n", 
    pu_GPP_FSP_Payload->send_pulse_all.HV_pos_count, pu_GPP_FSP_Payload->send_pulse_all.HV_neg_count);
    UART_Printf(&RF_UART, "POS HV PULSE COUNT: %d; NEG HV PULSE COUNT: %d\n", 
    pu_GPP_FSP_Payload->send_pulse_all.HV_pos_count, pu_GPP_FSP_Payload->send_pulse_all.HV_neg_count);

    UART_Printf(&RS232_UART, "> POS LV PULSE COUNT: %d; NEG LV PULSE COUNT: %d\n", 
    pu_GPP_FSP_Payload->send_pulse_all.LV_pos_count, pu_GPP_FSP_Payload->send_pulse_all.LV_neg_count);
    UART_Printf(&RF_UART, "> POS LV PULSE COUNT: %d; NEG LV PULSE COUNT: %d\n", 
    pu_GPP_FSP_Payload->send_pulse_all.LV_pos_count, pu_GPP_FSP_Payload->send_pulse_all.LV_neg_count);

    UART_Printf(&RS232_UART, "> DELAY BETWEEN HV POS AND NEG PULSE: %dms\n", 
    pu_GPP_FSP_Payload->send_pulse_all.HV_delay);
    UART_Printf(&RF_UART, "> DELAY BETWEEN HV POS AND NEG PULSE: %dms\n", 
    pu_GPP_FSP_Payload->send_pulse_all.HV_delay);

    UART_Printf(&RS232_UART, "> DELAY BETWEEN LV POS AND NEG PULSE: %dms\n", 
    pu_GPP_FSP_Payload->send_pulse_all.LV_delay);
    UART_Printf(&RF_UART, "> DELAY BETWEEN LV POS AND NEG PULSE: %dms\n", 
    pu_GPP_FSP_Payload->send_pulse_all.LV_delay);

    delay_ms = pu_GPP_FSP_Payload->send_pulse_all.Delay_high;
    delay_ms <<= 8;
    delay_ms |= pu_GPP_FSP_Payload->send_pulse_all.Delay_low;
    UART_Printf(&RS232_UART, "> DELAY BETWEEN HV PULSE AND LV PULSE: %dms\n", delay_ms);
    UART_Printf(&RF_UART, "> DELAY BETWEEN HV PULSE AND LV PULSE: %dms\n", delay_ms);

    UART_Printf(&RS232_UART, "> HV PULSE ON TIME: %dms; HV PULSE OFF TIME: %dms\n", 
    pu_GPP_FSP_Payload->send_pulse_all.OnTime, pu_GPP_FSP_Payload->send_pulse_all.OffTime);
    UART_Printf(&RF_UART, "> HV PULSE ON TIME: %dms; HV PULSE OFF TIME: %dms\n", 
    pu_GPP_FSP_Payload->send_pulse_all.OnTime, pu_GPP_FSP_Payload->send_pulse_all.OffTime);

    lv_on_time_ms = pu_GPP_FSP_Payload->send_pulse_all.OnTime_high;
    lv_on_time_ms <<= 8;
    lv_on_time_ms |= pu_GPP_FSP_Payload->send_pulse_all.OnTime_low;

    lv_off_time_ms = pu_GPP_FSP_Payload->send_pulse_all.OffTime_high;
    lv_off_time_ms <<= 8;
    lv_off_time_ms |= pu_GPP_FSP_Payload->send_pulse_all.OffTime_low;

    UART_Printf(&RS232_UART, "> LV PULSE ON TIME: %dms; LV PULSE OFF TIME: %dms\n", lv_on_time_ms, lv_off_time_ms);
    UART_Printf(&RF_UART, "> LV PULSE ON TIME: %dms; LV PULSE OFF TIME: %dms\n", lv_on_time_ms, lv_off_time_ms);

    if (pu_GPP_FSP_Payload->send_pulse_all.State == 1)
    {
        UART_Send_String(&RS232_UART, "> H BRIDGE IS PULSING\n");
        UART_Send_String(&RF_UART, "> H BRIDGE IS PULSING\n");
    }
    else
    {
        UART_Send_String(&RS232_UART, "> H BRIDGE IS NOT PULSING\n");
        UART_Send_String(&RF_UART, "> H BRIDGE IS NOT PULSING\n");
    }

    UART_Send_String(&RS232_UART, "> ");
    UART_Send_String(&RF_UART, "> ");

    break;
}
    

case FSP_CMD_SEND_RELAY_POLE:
{
    UART_Printf(&RS232_UART, "RELAY POS POLE: %d; RELAY NEG POLE: %d\n", 
    pu_GPP_FSP_Payload->send_relay_pole.HvRelay, pu_GPP_FSP_Payload->send_relay_pole.LvRelay);

    UART_Printf(&RF_UART, "RELAY POS POLE: %d; RELAY NEG POLE: %d\n", 
    pu_GPP_FSP_Payload->send_relay_pole.HvRelay, pu_GPP_FSP_Payload->send_relay_pole.LvRelay);

    UART_Send_String(&RS232_UART, "> ");
    UART_Send_String(&RF_UART, "> ");
}
    
    break;
case FSP_CMD_SEND_RELAY_CONTROL:
{
    if (pu_GPP_FSP_Payload->send_relay_control.State == 1)
    {
        UART_Send_String(&RS232_UART, "RELAY IS ON\n");
        UART_Send_String(&RF_UART, "RELAY IS ON\n");
    }
    else
    {
        UART_Send_String(&RS232_UART, "RELAY IS OFF\n");
        UART_Send_String(&RF_UART, "RELAY IS OFF\n");
    }

    UART_Send_String(&RS232_UART, "> ");
    UART_Send_String(&RF_UART, "> ");
    
    break;
}
    
case FSP_CMD_SEND_RELAY_ALL:
{
    UART_Printf(&RS232_UART, "RELAY POS POLE: %d; RELAY NEG POLE: %d\n", 
    pu_GPP_FSP_Payload->send_relay_all.HvRelay, pu_GPP_FSP_Payload->send_relay_all.LvRelay);

    UART_Printf(&RF_UART, "RELAY POS POLE: %d; RELAY NEG POLE: %d\n", 
    pu_GPP_FSP_Payload->send_relay_all.HvRelay, pu_GPP_FSP_Payload->send_relay_all.LvRelay);

    if (pu_GPP_FSP_Payload->send_relay_all.State == 1)
    {
        UART_Send_String(&RS232_UART, "> RELAY IS ON\n");
        UART_Send_String(&RF_UART, "> RELAY IS ON\n");
    }
    else
    {
        UART_Send_String(&RS232_UART, "> RELAY IS OFF\n");
        UART_Send_String(&RF_UART, "> RELAY IS OFF\n");
    }

    UART_Send_String(&RS232_UART, "> ");
    UART_Send_String(&RF_UART, "> ");
    break;
}
    

case FSP_CMD_SENT_CURRENT:
{
    Avr_Current = pu_GPP_FSP_Payload->get_current.Value_high;
    Avr_Current = Avr_Current << 8;
    Avr_Current |= pu_GPP_FSP_Payload->get_current.Value_low;

    if (Avr_Current < 1000)
    {
        UART_Printf(&RS232_UART, "CURRENT IS %dmA\n", Avr_Current);
        UART_Printf(&RF_UART, "CURRENT IS %dmA\n", Avr_Current);

        UART_Send_String(&RS232_UART, "> ");
        UART_Send_String(&RF_UART, "> ");
    }
    else
    {
        UART_Printf(&RS232_UART, "CURRENT IS %d.%dA\n", Avr_Current / 1000, Avr_Current % 1000);
        UART_Printf(&RF_UART, "CURRENT IS %d.%dA\n", Avr_Current / 1000, Avr_Current % 1000);

        UART_Send_String(&RS232_UART, "> ");
        UART_Send_String(&RF_UART, "> ");
    }
    
    break;
}
    

case FSP_CMD_SENT_IMPEDANCE:
{
    Voltage = g_Feedback_Voltage[0] * 1000 / hv_calib_coefficient.average_value;

    PID_is_300V_on = 0;
    PID_is_50V_on = 0;
    g_is_Discharge_300V_On = 1;
    g_is_Discharge_50V_On = 1;	

    Avr_Current = pu_GPP_FSP_Payload->get_impedance.Value_high;
    Avr_Current = Avr_Current << 8;
    Avr_Current |= pu_GPP_FSP_Payload->get_impedance.Value_low;

    Impedance = Voltage / Avr_Current;

    UART_Send_String(&RS232_UART, "MEASURING...OK\n");
    UART_Send_String(&RF_UART, "MEASURING...OK\n");
    
    UART_Printf(&RS232_UART, "> CURRENT IS %dmA\n", Avr_Current);
    UART_Printf(&RF_UART, "> CURRENT IS %dmA\n", Avr_Current);

    UART_Printf(&RS232_UART, "> IMPEDANCE IS %d Ohm\n", Impedance);
    UART_Printf(&RF_UART, "> IMPEDANCE IS %d Ohm\n", Impedance);

    UART_Send_String(&RS232_UART, "> ");
    UART_Send_String(&RF_UART, "> ");
    break;
}
    
case FSP_CMD_GET_BMP390	:
{
    UART_Send_String(&RS232_UART, "Received FSP_CMD_GET_BMP390\n");
    UART_Send_String(&RF_UART, "Received FSP_CMD_GET_BMP390\n");

    UART_Printf(&RS232_UART, "> TEMPERATURE: %s Celsius\n", pu_GPP_FSP_Payload->getBMP390.temp);
    UART_Printf(&RF_UART, "> TEMPERATURE: %s Celsius\n", pu_GPP_FSP_Payload->getBMP390.temp);

    UART_Printf(&RS232_UART, "> PRESSURE: %s Pa\n", pu_GPP_FSP_Payload->getBMP390.pressure);
    UART_Printf(&RF_UART, "> PRESSURE: %s Pa\n", pu_GPP_FSP_Payload->getBMP390.pressure);

    UART_Send_String(&RS232_UART, "> ");
    UART_Send_String(&RF_UART, "> ");
    break;
}
    
case FSP_CMD_GET_LMSDOX:
{
    UART_Send_String(&RS232_UART, "Received FSP_CMD_GET_LSMDOX\n");
    UART_Send_String(&RF_UART, "Received FSP_CMD_GET_LSMDOX\n");

    uint32_t accel_x=0, accel_y=0, accel_z=0, gyro_x=0, gyro_y=0, gyro_z=0;

    convertArrayToInteger(pu_GPP_FSP_Payload->getLSMDOX.accel_x, &accel_x);
    convertArrayToInteger(pu_GPP_FSP_Payload->getLSMDOX.accel_y, &accel_y);
    convertArrayToInteger(pu_GPP_FSP_Payload->getLSMDOX.accel_z, &accel_z);
    convertArrayToInteger(pu_GPP_FSP_Payload->getLSMDOX.gyro_x, &gyro_x);
    convertArrayToInteger(pu_GPP_FSP_Payload->getLSMDOX.gyro_y, &gyro_y);
    convertArrayToInteger(pu_GPP_FSP_Payload->getLSMDOX.gyro_z, &gyro_z);

    UART_Printf(&RS232_UART, "> ACCEL x: %dmm/s2; ACCEL y: %dmm/s2; ACCEL z: %dmm/s2\n", accel_x, accel_y, accel_z);
    UART_Printf(&RF_UART, "> ACCEL x: %dmm/s2; ACCEL y: %dmm/s2; ACCEL z: %dmm/s2\n", accel_x, accel_y, accel_z);

    UART_Printf(&RS232_UART, "> GYRO x: %dmpds; GYRO y: %dmpds; GYRO z: %dmpds\n", gyro_x, gyro_y, gyro_z);
    UART_Printf(&RF_UART, "> GYRO x: %dmpds; GYRO y: %dmpds; GYRO z: %dmpds\n", gyro_x, gyro_y, gyro_z);

    UART_Send_String(&RS232_UART, "> ");
    UART_Send_String(&RF_UART, "> ");
    break;
}
    
default:
    break;
}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void convertArrayToInteger(uint8_t arr[], uint32_t *p)
{
	*p= (uint32_t)(arr[3]<<24 | arr[2]<<16 | arr[1]<<8 | arr[0]);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */