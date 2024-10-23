/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "app.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
struct _fsp_line_typedef {
	uint16_t buffer_size;
	char *p_buffer;

	volatile uint16_t write_index;
	volatile char RX_char;
};
typedef struct _fsp_line_typedef fsp_line_typedef;
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
//static const char * ErrorCode[7] =
//{
//    "OK\n",
//    "FSP_PKT_NOT_READY\n",
//    "FSP_PKT_INVALID\n",
//    "FSP_PKT_WRONG_ADR\n",
//    "FSP_PKT_ERROR\n",
//    "FSP_PKT_CRC_FAIL\n",
//    "FSP_PKT_WRONG_LENGTH\n"
//};
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
extern 	uart_stdio_typedef RS232_UART;
extern 	uart_stdio_typedef RF_UART;
		uart_stdio_typedef GPP_UART;

char g_GPP_UART_TX_buffer[GPP_TX_SIZE];
char g_GPP_UART_RX_buffer[GPP_RX_SIZE];
float temperature;
uint32_t pressure;
fsp_packet_t 	s_GPC_FSP_Packet;
fsp_packet_t 	s_GPP_FSP_Packet;
GPC_FSP_Payload *pu_GPC_FSP_Payload;		//for TX
GPP_FSP_Payload *pu_GPP_FSP_Payload;		//for RX

fsp_line_typedef FSP_line;
char g_FSP_line_buffer[FSP_BUF_LEN];

bool is_receive_SOD = false;
bool escape = false;
void convertArrayToInteger(uint8_t arr[], uint32_t *p);

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: CMD Line Task Init :::::::: */
void FSP_Line_Task_Init() {
	UART_Init(	&GPP_UART, GPP_UART_HANDLE, GPP_UART_IRQ, g_GPP_UART_TX_buffer,
				g_GPP_UART_RX_buffer, GPP_TX_SIZE, GPP_RX_SIZE);

	FSP_line.p_buffer 		= g_FSP_line_buffer;
	FSP_line.buffer_size 	= FSP_BUF_LEN;
	FSP_line.write_index 	= 0;

	pu_GPC_FSP_Payload = (GPC_FSP_Payload*) (&s_GPC_FSP_Packet.payload);
	pu_GPP_FSP_Payload = (GPP_FSP_Payload*) (&s_GPP_FSP_Packet.payload);

	if (FSP_line.buffer_size != 0) {
		memset((void*) FSP_line.p_buffer, 0, sizeof(FSP_line.p_buffer));
	}
	fsp_init(FSP_ADR_GPC);
}

/* :::::::::: CMD Line Task ::::::::::::: */

void FSP_Line_Task(void)
{
	uint8_t time_out = 0;

	for (time_out = 50, escape = false; (!RX_BUFFER_EMPTY(&GPP_UART)) && (time_out != 0) && (escape == false); time_out--)
	{
		FSP_line.RX_char = UART_Get_Char(&GPP_UART);
		escape = false;

		if (FSP_line.RX_char == FSP_PKT_SOD)
		{
			FSP_line.write_index = 0;
			is_receive_SOD = true;
		}
		else if ((FSP_line.RX_char == FSP_PKT_EOF) && (is_receive_SOD == true))
		{
			switch (frame_decode((uint8_t*) FSP_line.p_buffer, FSP_line.write_index, &s_GPP_FSP_Packet))
			{
			//process command
			case FSP_PKT_NOT_READY:
				break;
			case FSP_PKT_READY:
				UART_Send_String(&RS232_UART, "Received FSP packet\r\n");
				FSP_Line_Process();
				break;
			case FSP_PKT_INVALID:

				break;
			case FSP_PKT_WRONG_ADR:
				UART_Send_String(&RS232_UART, "Wrong module adr\r\n");

				break;
			case FSP_PKT_ERROR:
				UART_Send_String(&RS232_UART, "Packet error\r\n");

				break;
			case FSP_PKT_CRC_FAIL:
				UART_Send_String(&RS232_UART, "CRC error\r\n");

				break;
			default:

				break;
			}
			FSP_line.write_index = 0;
			is_receive_SOD = false;

			escape = true;
		}
		else 
		{
			FSP_line.p_buffer[FSP_line.write_index] = FSP_line.RX_char;
			FSP_line.write_index++;

			if (FSP_line.write_index > FSP_line.buffer_size)
				FSP_line.write_index = 0;

		}
	}
}

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
	case FSP_CMD_SEND_PULSE_DELAY:
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
	case FSP_CMD_SEND_PULSE_HV:
		UART_Printf(&RS232_UART, "HV PULSE ON TIME: %dms; HV PULSE OFF TIME: %dms\n", 
		pu_GPP_FSP_Payload->send_pulse_HV.OnTime, pu_GPP_FSP_Payload->send_pulse_HV.OffTime);
		UART_Printf(&RF_UART, "HV PULSE ON TIME: %dms; HV PULSE OFF TIME: %dms\n", 
		pu_GPP_FSP_Payload->send_pulse_HV.OnTime, pu_GPP_FSP_Payload->send_pulse_HV.OffTime);

		UART_Send_String(&RS232_UART, "> ");
		UART_Send_String(&RF_UART, "> ");
		break;
	case FSP_CMD_SEND_PULSE_LV:
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
	case FSP_CMD_SEND_PULSE_CONTROL:
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
	case FSP_CMD_SEND_PULSE_ALL:
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

	case FSP_CMD_SEND_RELAY_POLE:
		UART_Printf(&RS232_UART, "RELAY POS POLE: %d; RELAY NEG POLE: %d\n", 
		pu_GPP_FSP_Payload->send_relay_pole.HvRelay, pu_GPP_FSP_Payload->send_relay_pole.LvRelay);

		UART_Printf(&RF_UART, "RELAY POS POLE: %d; RELAY NEG POLE: %d\n", 
		pu_GPP_FSP_Payload->send_relay_pole.HvRelay, pu_GPP_FSP_Payload->send_relay_pole.LvRelay);

		UART_Send_String(&RS232_UART, "> ");
		UART_Send_String(&RF_UART, "> ");
		break;
	case FSP_CMD_SEND_RELAY_CONTROL:
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
	case FSP_CMD_SEND_RELAY_ALL:
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

	case FSP_CMD_SENT_CURRENT:
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

	case FSP_CMD_SENT_IMPEDANCE:
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
	case FSP_CMD_GET_BMP390	:
		UART_Send_String(&RS232_UART, "Received FSP_CMD_GET_BMP390\n");
		UART_Send_String(&RF_UART, "Received FSP_CMD_GET_BMP390\n");

		UART_Printf(&RS232_UART, "> TEMPERATURE: %s Celsius\n", pu_GPP_FSP_Payload->getBMP390.temp);
		UART_Printf(&RF_UART, "> TEMPERATURE: %s Celsius\n", pu_GPP_FSP_Payload->getBMP390.temp);

		UART_Printf(&RS232_UART, "> PRESSURE: %s Pa\n", pu_GPP_FSP_Payload->getBMP390.pressure);
		UART_Printf(&RF_UART, "> PRESSURE: %s Pa\n", pu_GPP_FSP_Payload->getBMP390.pressure);

		UART_Send_String(&RS232_UART, "> ");
		UART_Send_String(&RF_UART, "> ");
		break;
	case FSP_CMD_GET_LMSDOX:
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
	default:
		break;
	}
}

/* :::::::::: IRQ Handler ::::::::::::: */
void GPP_UART_IRQHandler(void) {
	if (LL_USART_IsActiveFlag_TXE(GPP_UART.handle) == true) {
		if (TX_BUFFER_EMPTY(&GPP_UART))
		{
			// Buffer empty, so disable interrupts
			LL_USART_DisableIT_TXE(GPP_UART.handle);
		} 
		else
		{
			// There is more data in the output buffer. Send the next byte
			UART_Prime_Transmit(&GPP_UART);
		}
	}

	if (LL_USART_IsActiveFlag_RXNE(GPP_UART.handle) == true)
	{
		GPP_UART.RX_irq_char = LL_USART_ReceiveData8(GPP_UART.handle);

		// NOTE: On win 10, default PUTTY when hit enter only send back '\r',
		// while on default HERCULES when hit enter send '\r\n' in that order.
		// The code bellow is modified so that it can work on PUTTY and HERCULES.
		if ((!RX_BUFFER_FULL(&GPP_UART))) 
		{
			GPP_UART.p_RX_buffer[GPP_UART.RX_write_index] = GPP_UART.RX_irq_char;
			ADVANCE_RX_WRITE_INDEX(&GPP_UART);
		}
	}
}
void convertArrayToInteger(uint8_t arr[], uint32_t *p)
{
	*p= (uint32_t)(arr[3]<<24 | arr[2]<<16 | arr[1]<<8 | arr[0]);
}
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
