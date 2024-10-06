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

fsp_packet_t 	s_GPC_FSP_Packet;
fsp_packet_t 	s_GPP_FSP_Packet;
GPC_FSP_Payload *pu_GPC_FSP_Payload;		//for TX
GPP_FSP_Payload *pu_GPP_FSP_Payload;		//for RX

fsp_line_typedef FSP_line;
char g_FSP_line_buffer[FSP_BUF_LEN];

bool is_receive_SOD = false;
bool escape = false;

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
void FSP_Line_Process()
{
	switch (s_GPC_FSP_Packet.type) {
	case FSP_PKT_TYPE_DATA:

		break;
	case FSP_PKT_TYPE_DATA_WITH_ACK:

		break;
	case FSP_PKT_TYPE_CMD:

		break;
	case FSP_PKT_TYPE_CMD_WITH_ACK:

		break;
	case FSP_PKT_TYPE_ACK:

		break;
	case FSP_PKT_TYPE_NACK:

		break;
	case FSP_PKT_TYPE_CMD_W_DATA:
		switch (pu_GPP_FSP_Payload->commonFrame.Cmd) {
		case FSP_CMD_AVR_CURRENT:
			Voltage = g_Feedback_Voltage[0] * 125;
			PID_is_300V_on = 0;
			PID_is_50V_on = 0;
			g_is_Discharge_300V_On = 1;
			g_is_Discharge_50V_On = 1;	
			UART_Send_String(&RS232_UART, "> RECEIVED FSP_CMD_IMPDANCE\r\n");
			UART_Send_String(&RF_UART, "> RECEIVED FSP_CMD_IMPDANCE\r\n");
			Avr_Current = pu_GPP_FSP_Payload->avr_current.Value_high;
			Avr_Current = Avr_Current << 8;
			Avr_Current |= pu_GPP_FSP_Payload->avr_current.Value_low;
			Impedance = Voltage / Avr_Current;
			UART_Send_String(&RF_UART, "> MEASURING...OK\n");
			UART_Printf(&RF_UART, "> IMPEDANCE IS %d Ohm\n", Impedance);
			UART_Send_String(&RF_UART, "> ");
			break;
		
		default:
			break;
		}
		break;
	case FSP_PKT_TYPE_CMD_W_DATA_ACK:

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

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
