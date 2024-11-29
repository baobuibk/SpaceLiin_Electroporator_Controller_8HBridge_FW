/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "command.h"
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static bool			is_h_bridge_enable;

static uint16_t		pulse_delay_ms;
static uint8_t		HB_pos_pole_index;
static uint8_t		HB_neg_pole_index;

static uint8_t		hv_pulse_pos_count;
static uint8_t		hv_pulse_neg_count;
static uint8_t		hv_delay_ms;
static uint8_t		hv_on_time_ms;
static uint8_t		hv_off_time_ms;

static uint8_t		lv_pulse_pos_count;
static uint8_t		lv_pulse_neg_count;
static uint8_t		lv_delay_ms;
static uint16_t		lv_on_time_ms;
static uint16_t		lv_off_time_ms;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void fsp_print(uint8_t packet_length);
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
extern uart_stdio_typedef *CMD_line_handle;

tCmdLineEntry g_psCmdTable[] = {
		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Cap Control Command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		{ "SET_CAP_VOLT",			CMD_SET_CAP_VOLT, 			" : Set cap voltage" },
		{ "SET_CAP_CONTROL",		CMD_SET_CAP_CONTROL, 		" : Control charger on/off" },
		{ "SET_CAP_RELEASE",		CMD_SET_CAP_RELEASE, 		" : Control releasing cap" },

		{ "GET_CAP_VOLT",			CMD_GET_CAP_VOLT, 			" : Get set voltage on cap" },
		{ "GET_CAP_CONTROL",		CMD_GET_CAP_CONTROL, 		" : Get info whether cap is charging or not" },
		{ "GET_CAP_RELEASE",		CMD_GET_CAP_RELEASE, 		" : Get info whether cap is releasing or not" },
		{ "GET_CAP_ALL",			CMD_GET_CAP_ALL, 			" : Get all info about cap" },

		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Pulse Control Command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		{ "SET_PULSE_POLE",			CMD_SET_PULSE_POLE,			" : Set pos and neg pole of a pulse" },
		{ "SET_PULSE_COUNT",		CMD_SET_PULSE_COUNT, 		" : Set number of pulse" },
		{ "SET_PULSE_DELAY",		CMD_SET_PULSE_DELAY, 		" : Set delay between pulse hv and lv" },
		{ "SET_PULSE_HV", 			CMD_SET_PULSE_HV, 			" : Set hs pulse on time and off time" },
		{ "SET_PULSE_LV", 			CMD_SET_PULSE_LV, 			" : Set ls pulse on time and off time" },
		{ "SET_PULSE_CONTROL", 		CMD_SET_PULSE_CONTROL, 		" : Start pulsing" },

		{ "GET_PULSE_POLE",			CMD_GET_PULSE_POLE,			" : Get the pos and neg pole of a pulse" },
		{ "GET_PULSE_COUNT",		CMD_GET_PULSE_COUNT, 		" : Get number of pulse" },
		{ "GET_PULSE_DELAY",		CMD_GET_PULSE_DELAY, 		" : Get delay between pulse hv and lv" },
		{ "GET_PULSE_HV", 			CMD_GET_PULSE_HV, 			" : Get hs pulse on time and off time" },
		{ "GET_PULSE_LV", 			CMD_GET_PULSE_LV, 			" : Get ls pulse on time and off time" },
		{ "GET_PULSE_CONTROL", 		CMD_GET_PULSE_CONTROL, 		" : Get info whether pulse starting pulsing" },
		{ "GET_PULSE_ALL", 			CMD_GET_PULSE_ALL, 			" : Get all info about pulse" },

		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ VOM Command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		{ "MEASURE_VOLT", 			CMD_MEASURE_VOLT,			" : Measure cap voltage"},
		{ "MEASURE_CURRENT",		CMD_MEASURE_CURRENT,		" : Measure cuvette current"},
		{ "MEASURE_IMPEDANCE", 		CMD_MEASURE_IMPEDANCE,		" : Measure cuvette impedance"},

		/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Ultility Command ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
		{ "HELP", 					CMD_HELP,					" : Display list of commands" },
		{ "CALIB_RUN", 				CMD_CALIB_RUN, 				" : Start cap calib process" },
		{ "CALIB_MEASURE",			CMD_CALIB_MEASURE, 			" : Command to input VOM value" },
		{ "CALL_GPP", 				CMD_CALL_GPP,	    		" : Test communicate to GPP" },
		{ "GET_BMP390", 			CMD_GET_BMP390,	    		" : Get temperature and pressure" },
		{ "GET_LMSDOX", 			CMD_GET_LMSDOX,	   			 " : Get accel and gyro" },
		{ 0, 0, 0 }
};
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: Cap Control Command :::::::: */
int CMD_SET_CAP_VOLT(int argc, char *argv[])
{
	if (g_is_calib_running == true)
	{
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	if ((receive_argm[0] > 300) || (receive_argm[0] < 0))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[1] > 50) || (receive_argm[1] < 0))
		return CMDLINE_INVALID_ARG;

	Calib_Calculate(receive_argm[0], receive_argm[1]);

	return CMDLINE_OK;
}

int CMD_SET_CAP_CONTROL(int argc, char *argv[])
{
	if (g_is_calib_running == true)
	{
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	if ((receive_argm[0] > 1) || (receive_argm[0] < 0))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[1] > 1) || (receive_argm[1] < 0))
		return CMDLINE_INVALID_ARG;

	if (receive_argm[0] == 1) {
		g_is_Discharge_300V_On = 0;
	}

	if (receive_argm[1] == 1) {
		g_is_Discharge_50V_On = 0;
	}

	PID_is_300V_on = receive_argm[0];
	PID_is_50V_on = receive_argm[1];

	return CMDLINE_OK;
}

int CMD_SET_CAP_RELEASE(int argc, char *argv[])
{
	if (g_is_calib_running == true) {
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	if ((receive_argm[0] > 1) || (receive_argm[0] < 0))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[1] > 1) || (receive_argm[1] < 0))
		return CMDLINE_INVALID_ARG;

	if (receive_argm[0] == 1) {
		PID_is_300V_on = 0;
	}

	if (receive_argm[1] == 1) {
		PID_is_50V_on = 0;
	}

	g_is_Discharge_300V_On = receive_argm[0];
	g_is_Discharge_50V_On = receive_argm[1];

	return CMDLINE_OK;
}

int CMD_GET_CAP_VOLT(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	uint16_t hv_cap_set_voltage, lv_cap_set_voltage;
	hv_cap_set_voltage = PID_300V_set_voltage / hv_calib_coefficient.average_value;
	lv_cap_set_voltage = PID_50V_set_voltage / lv_calib_coefficient.average_value;

	UART_Printf(CMD_line_handle, "> HV CAP IS SET AT: %dV, LV CAP IS SET AT: %dV\n", hv_cap_set_voltage, lv_cap_set_voltage);

	return CMDLINE_OK;
}

int CMD_GET_CAP_CONTROL(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	if (PID_is_300V_on == true)
	{
		UART_Send_String(CMD_line_handle, "> HV CAP IS CHARGING\n");
	}
	else
	{
		UART_Send_String(CMD_line_handle, "> HV CAP IS NOT CHARGING\n");
	}

	if (PID_is_50V_on == true)
	{
		UART_Send_String(CMD_line_handle, "> LV CAP IS CHARGING\n");
	}
	else
	{
		UART_Send_String(CMD_line_handle, "> LV CAP IS NOT CHARGING\n");
	}
	
	return CMDLINE_OK;
}

int CMD_GET_CAP_RELEASE(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	if (g_is_Discharge_300V_On == true)
	{
		UART_Send_String(CMD_line_handle, "> HV CAP IS DISCHARGING\n");
	}
	else
	{
		UART_Send_String(CMD_line_handle, "> HV CAP IS NOT DISCHARGING\n");
	}

	if (g_is_Discharge_50V_On == true)
	{
		UART_Send_String(CMD_line_handle, "> LV CAP IS DISCHARGING\n");
	}
	else
	{
		UART_Send_String(CMD_line_handle, "> LV CAP IS NOT DISCHARGING\n");
	}

	return CMDLINE_OK;
}

int CMD_GET_CAP_ALL(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;
	
	uint16_t hv_cap_set_voltage, lv_cap_set_voltage;
	hv_cap_set_voltage = PID_300V_set_voltage / hv_calib_coefficient.average_value;
	lv_cap_set_voltage = PID_50V_set_voltage / lv_calib_coefficient.average_value;

	UART_Printf(CMD_line_handle, "> HV CAP IS SET AT: %dV, LV CAP IS SET AT: %dV\n", hv_cap_set_voltage, lv_cap_set_voltage);

	if (PID_is_300V_on == true)
	{
		UART_Send_String(CMD_line_handle, "> HV CAP IS CHARGING\n");
	}
	else
	{
		UART_Send_String(CMD_line_handle, "> HV CAP IS NOT CHARGING\n");
	}

	if (PID_is_50V_on == true)
	{
		UART_Send_String(CMD_line_handle, "> LV CAP IS CHARGING\n");
	}
	else
	{
		UART_Send_String(CMD_line_handle, "> LV CAP IS NOT CHARGING\n");
	}

	if (g_is_Discharge_300V_On == true)
	{
		UART_Send_String(CMD_line_handle, "> HV CAP IS DISCHARGING\n");
	}
	else
	{
		UART_Send_String(CMD_line_handle, "> HV CAP IS NOT DISCHARGING\n");
	}

	if (g_is_Discharge_50V_On == true)
	{
		UART_Send_String(CMD_line_handle, "> LV CAP IS DISCHARGING\n");
	}
	else
	{
		UART_Send_String(CMD_line_handle, "> LV CAP IS NOT DISCHARGING\n");
	}

	return CMDLINE_OK;
}

/* :::::::::: Pulse Control Command :::::::: */
int CMD_SET_PULSE_POLE(int argc, char *argv[])
{
	if (g_is_calib_running == true) {
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[3];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	if ((receive_argm[0] > 9) || (receive_argm[1] > 9))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[0] < 1) || (receive_argm[1] < 1))
		return CMDLINE_INVALID_ARG;

	HB_pos_pole_index = receive_argm[0];
	HB_neg_pole_index = receive_argm[1];

	ps_FSP_TX->CMD	   						   = FSP_CMD_SET_PULSE_POLE;
	ps_FSP_TX->Payload.set_pulse_pole.pos_pole = receive_argm[0];
	ps_FSP_TX->Payload.set_pulse_pole.neg_pole = receive_argm[1];

	fsp_print(3);
	return CMDLINE_OK;
}

int CMD_SET_PULSE_COUNT(int argc, char *argv[])
{
	if (g_is_calib_running == true) {
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 5)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 5)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[4];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);
	receive_argm[2] = atoi(argv[3]);
	receive_argm[3] = atoi(argv[4]);

	if ((receive_argm[0] > 20) || (receive_argm[1] > 20) || (receive_argm[2] > 20) || (receive_argm[3] > 20))
		return CMDLINE_INVALID_ARG;

	hv_pulse_pos_count = receive_argm[0];
	hv_pulse_neg_count = receive_argm[1];

	lv_pulse_pos_count = receive_argm[2];
	lv_pulse_neg_count = receive_argm[3];

	ps_FSP_TX->CMD	   						 		 = FSP_CMD_SET_PULSE_COUNT;
	ps_FSP_TX->Payload.set_pulse_count.HV_pos_count = receive_argm[0];
	ps_FSP_TX->Payload.set_pulse_count.HV_neg_count = receive_argm[1];

	ps_FSP_TX->Payload.set_pulse_count.LV_pos_count = receive_argm[2];
	ps_FSP_TX->Payload.set_pulse_count.LV_neg_count = receive_argm[3];

	fsp_print(5);
	return CMDLINE_OK;
}

int CMD_SET_PULSE_DELAY(int argc, char *argv[])
{
	if (g_is_calib_running == true) {
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 4)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 4)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[3];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);
	receive_argm[2] = atoi(argv[3]);

	if ((receive_argm[0] > 100) || (receive_argm[0] < 0))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[1] > 100) || (receive_argm[1] < 0))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[2] > 1000) || (receive_argm[2] < 0))
		return CMDLINE_INVALID_ARG;

	hv_delay_ms = receive_argm[0];
	lv_delay_ms = receive_argm[1];

	pulse_delay_ms = receive_argm[2];

	ps_FSP_TX->CMD									= FSP_CMD_SET_PULSE_DELAY;
	ps_FSP_TX->Payload.set_pulse_delay.HV_delay	= receive_argm[0];
	ps_FSP_TX->Payload.set_pulse_delay.LV_delay	= receive_argm[1];

	ps_FSP_TX->Payload.set_pulse_delay.Delay_low 	= receive_argm[2];
	ps_FSP_TX->Payload.set_pulse_delay.Delay_high 	= (receive_argm[2] >> 8);

	fsp_print(5);
	return CMDLINE_OK;
}

int CMD_SET_PULSE_HV(int argc, char *argv[])
{
	if (g_is_calib_running == true) {
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	if ((receive_argm[0] > 20) || (receive_argm[0] < 1))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[1] > 20) || (receive_argm[1] < 1))
		return CMDLINE_INVALID_ARG;

	hv_on_time_ms  = receive_argm[0];
	hv_off_time_ms = receive_argm[1];

	ps_FSP_TX->CMD 	 			 		 = FSP_CMD_SET_PULSE_HV;
	ps_FSP_TX->Payload.set_pulse_HV.OnTime	 = receive_argm[0];
	ps_FSP_TX->Payload.set_pulse_HV.OffTime = receive_argm[1];

	fsp_print(3);
	return CMDLINE_OK;
}

int CMD_SET_PULSE_LV(int argc, char *argv[])
{
	if (g_is_calib_running == true) {
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	if ((receive_argm[0] > 1000) || (receive_argm[0] < 1))
		return CMDLINE_INVALID_ARG;
	else if ((receive_argm[1] > 1000) || (receive_argm[1] < 1))
		return CMDLINE_INVALID_ARG;

	lv_on_time_ms  = receive_argm[0];
	lv_off_time_ms = receive_argm[1];

	ps_FSP_TX->CMD 								= FSP_CMD_SET_PULSE_LV;
	ps_FSP_TX->Payload.set_pulse_LV.OnTime_low 	= receive_argm[0];
	ps_FSP_TX->Payload.set_pulse_LV.OnTime_high 	= (receive_argm[0] >> 8);

	ps_FSP_TX->Payload.set_pulse_LV.OffTime_low 	= receive_argm[1];
	ps_FSP_TX->Payload.set_pulse_LV.OffTime_high 	= (receive_argm[1] >> 8);

	fsp_print(5);
	return CMDLINE_OK;
}

int CMD_SET_PULSE_CONTROL(int argc, char *argv[])
{
	if (g_is_calib_running == true) {
		return CMDLINE_CALIB_IS_RUNNING;
	}

	if (argc < 2)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 2)
		return CMDLINE_TOO_MANY_ARGS;

	int8_t receive_argm = atoi(argv[1]);

	if ((receive_argm > 1) || (receive_argm < 0))
		return CMDLINE_INVALID_ARG;

	is_h_bridge_enable = receive_argm;

	ps_FSP_TX->CMD = FSP_CMD_SET_PULSE_CONTROL;
	ps_FSP_TX->Payload.set_pulse_control.State = receive_argm;

	fsp_print(2);
	return CMDLINE_OK;
}

int CMD_GET_PULSE_POLE(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(&RS232_UART, "> PULSE POS POLE: %d; PULSE NEG POLE: %d\n", 
	HB_pos_pole_index, HB_neg_pole_index);

	return CMDLINE_OK;
}

int CMD_GET_PULSE_COUNT(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(CMD_line_handle, "> POS HV PULSE COUNT: %d; NEG HV PULSE COUNT: %d\n", 
    hv_pulse_pos_count, hv_pulse_neg_count);

    UART_Printf(CMD_line_handle, "> POS LV PULSE COUNT: %d; NEG LV PULSE COUNT: %d\n", 
    lv_pulse_pos_count, lv_pulse_neg_count);

	return CMDLINE_OK;
}

int CMD_GET_PULSE_DELAY(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(CMD_line_handle, "> DELAY BETWEEN HV POS AND NEG PULSE: %dms\n", 
    hv_delay_ms);

    UART_Printf(CMD_line_handle, "> DELAY BETWEEN LV POS AND NEG PULSE: %dms\n", 
    lv_delay_ms);

    UART_Printf(CMD_line_handle, "> DELAY BETWEEN HV PULSE AND LV PULSE: %dms\n",
	pulse_delay_ms);

	return CMDLINE_OK;		
}

int CMD_GET_PULSE_HV(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(CMD_line_handle, "> HV PULSE ON TIME: %dms; HV PULSE OFF TIME: %dms\n", 
    hv_on_time_ms, hv_off_time_ms);

	return CMDLINE_OK;
}

int CMD_GET_PULSE_LV(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(CMD_line_handle, "> LV PULSE ON TIME: %dms; LV PULSE OFF TIME: %dms\n",
	lv_on_time_ms, lv_off_time_ms);
	
	return CMDLINE_OK;
}

int CMD_GET_PULSE_CONTROL(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	if (is_h_bridge_enable == 1)
    {
        UART_Send_String(CMD_line_handle, "> H BRIDGE IS PULSING\n");
    }
    else
    {
        UART_Send_String(CMD_line_handle, "> H BRIDGE IS NOT PULSING\n");
    }

	return CMDLINE_OK;
}

int CMD_GET_PULSE_ALL(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	UART_Printf(CMD_line_handle, "> POS HV PULSE COUNT: %d; NEG HV PULSE COUNT: %d\n", 
    hv_pulse_pos_count, hv_pulse_neg_count);

    UART_Printf(CMD_line_handle, "> POS LV PULSE COUNT: %d; NEG LV PULSE COUNT: %d\n", 
    lv_pulse_pos_count, lv_pulse_neg_count);

	UART_Printf(CMD_line_handle, "> DELAY BETWEEN HV POS AND NEG PULSE: %dms\n", 
    hv_delay_ms);

    UART_Printf(CMD_line_handle, "> DELAY BETWEEN LV POS AND NEG PULSE: %dms\n", 
    lv_delay_ms);

    UART_Printf(CMD_line_handle, "> DELAY BETWEEN HV PULSE AND LV PULSE: %dms\n",
	pulse_delay_ms);

	UART_Printf(CMD_line_handle, "> HV PULSE ON TIME: %dms; HV PULSE OFF TIME: %dms\n", 
    hv_on_time_ms, hv_off_time_ms);

	UART_Printf(CMD_line_handle, "> LV PULSE ON TIME: %dms; LV PULSE OFF TIME: %dms\n",
	lv_on_time_ms, lv_off_time_ms);

	if (is_h_bridge_enable == 1)
    {
        UART_Send_String(CMD_line_handle, "> H BRIDGE IS PULSING\n");
    }
    else
    {
        UART_Send_String(CMD_line_handle, "> H BRIDGE IS NOT PULSING\n");
    }

	return CMDLINE_OK;
}

/* :::::::::: VOM Command :::::::: */
int CMD_MEASURE_VOLT(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	uint32_t data_tmp[2];
	uint8_t data1_tmp[2];
	data_tmp[0] = g_Feedback_Voltage[0]* 10 / hv_calib_coefficient.average_value;
	data_tmp[1] = g_Feedback_Voltage[1]* 10 / lv_calib_coefficient.average_value;

	data1_tmp[0] = data_tmp[0] % 10;
	data1_tmp[1] = data_tmp[1] % 10;

	data_tmp[0] /= 10;
	data_tmp[1] /= 10;
	UART_Printf(&RS232_UART, "> HV cap: %d.%dV, LV cap: %d.%dV\r\n", data_tmp[0], data1_tmp[0], data_tmp[1], data1_tmp[1]);
	UART_Printf(&RF_UART, "> HV cap: %d.%dV, LV cap: %d.%dV\r\n", data_tmp[0], data1_tmp[0], data_tmp[1], data1_tmp[1]);

	return CMDLINE_OK;
}

int CMD_MEASURE_CURRENT(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	return CMDLINE_OK;
}

uint16_t Impedance_Period = 0;
int CMD_MEASURE_IMPEDANCE(int argc, char *argv[])
{
	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0]  = atoi(argv[1]);
	Impedance_Period = atoi(argv[2]);

	g_is_Discharge_300V_On = 0;
	g_is_Discharge_50V_On = 0;

	PID_is_300V_on = 0;
	PID_is_50V_on = 0;

	Calib_Calculate(receive_argm[0], 0);
	UART_Printf(&RF_UART, "> CHARGING HV TO %dV\r\n", receive_argm[0]);
	PID_is_300V_on = 1;

	SchedulerTaskEnable(7, 1);

	return CMDLINE_OK;
}

/* :::::::::: Ultility Command :::::::: */
int CMD_HELP(int argc, char *argv[])
{
	tCmdLineEntry *pEntry;

	UART_Send_String(&RS232_UART, "\nAvailable commands\r\n");
	UART_Send_String(&RS232_UART, "------------------\r\n");

	UART_Send_String(&RF_UART, "\nAvailable commands\r\n");
	UART_Send_String(&RF_UART, "------------------\r\n");

	// Point at the beginning of the command table.
	pEntry = &g_psCmdTable[0];

	// Enter a loop to read each entry from the command table.  The
	// end of the table has been reached when the command name is NULL.
	while (pEntry->pcCmd) {
		// Print the command name and the brief description.
		UART_Send_String(&RS232_UART, pEntry->pcCmd);
		UART_Send_String(&RS232_UART, pEntry->pcHelp);
		UART_Send_String(&RS232_UART, "\r\n");

		UART_Send_String(&RF_UART, pEntry->pcCmd);
		UART_Send_String(&RF_UART, pEntry->pcHelp);
		UART_Send_String(&RF_UART, "\r\n");

		// Advance to the next entry in the table.
		pEntry++;

	}
	// Return success.
	return (CMDLINE_OK);
}

int CMD_CALIB_RUN(int argc, char *argv[])
{
	if (argc < 2)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 2)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm = atoi(argv[1]);

	if ((receive_argm > 1) || (receive_argm < 0))
		return CMDLINE_INVALID_ARG;

	g_is_calib_running = receive_argm;

	if (receive_argm == 1) {
		SchedulerTaskEnable(6, 1);
	}

	return CMDLINE_OK;
}

int CMD_CALIB_MEASURE(int argc, char *argv[])
{
	if (argc < 3)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 3)
		return CMDLINE_TOO_MANY_ARGS;

	int receive_argm[2];

	receive_argm[0] = atoi(argv[1]);
	receive_argm[1] = atoi(argv[2]);

	g_HV_Measure_mv = receive_argm[0];
	g_LV_Measure_mv = receive_argm[1];

	g_is_measure_available = true;

	return CMDLINE_OK;
}

int CMD_CALL_GPP(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	ps_FSP_TX->CMD 			= FSP_CMD_HANDSHAKE;
	ps_FSP_TX->Payload.handshake.Check = 0xAB;

	fsp_print(2);
	return CMDLINE_OK;
}

int CMD_GET_BMP390(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	ps_FSP_TX->CMD = FSP_CMD_GET_BMP390;

	fsp_print(1);
	return CMDLINE_OK;
}

int CMD_GET_LMSDOX(int argc, char *argv[])
{
	if (argc < 1)
		return CMDLINE_TOO_FEW_ARGS;
	else if (argc > 1)
		return CMDLINE_TOO_MANY_ARGS;

	ps_FSP_TX->CMD = FSP_CMD_GET_LMSDOX;

	fsp_print(1);
	return CMDLINE_OK;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static void fsp_print(uint8_t packet_length)
{
	s_FSP_TX_Packet.sod 		= FSP_PKT_SOD;
	s_FSP_TX_Packet.src_adr 	= fsp_my_adr;
	s_FSP_TX_Packet.dst_adr 	= FSP_ADR_GPP;
	s_FSP_TX_Packet.length 	= packet_length;
	s_FSP_TX_Packet.type 		= FSP_PKT_TYPE_CMD_W_DATA;
	s_FSP_TX_Packet.eof 		= FSP_PKT_EOF;
	s_FSP_TX_Packet.crc16 		= crc16_CCITT(FSP_CRC16_INITIAL_VALUE, &s_FSP_TX_Packet.src_adr, s_FSP_TX_Packet.length + 4);

	uint8_t encoded_frame[25] = { 0 };
	uint8_t frame_len;
	fsp_encode(&s_FSP_TX_Packet, encoded_frame, &frame_len);

	UART_FSP(&GPP_UART, (char*)encoded_frame, frame_len);
}
