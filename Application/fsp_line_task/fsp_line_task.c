/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "fsp_line_task.h"

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
extern uart_stdio_typedef RS232_UART;
uart_stdio_typedef GPP_UART;

char g_GPP_UART_TX_buffer[GPP_TX_BUF_LEN];
char g_GPP_UART_RX_buffer[GPP_RX_BUF_LEN];

fsp_packet_t s_GPC_FspPacket;
fsp_packet_t s_GPP_FspPacket;
GPC_Sfp_Payload *s_pGPC_Sfp_Payload;		//for TX
GPP_Sfp_Payload *s_pGPP_Sfp_Payload;		//for RX

fsp_line_typedef FSP_line;
char g_FSP_line_buffer[FSP_BUF_LEN];

bool is_receive_SOD = false;
bool escape = false;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: CMD Line Task Init :::::::: */
void FSP_Line_Task_Init() {
	UART_Init(&GPP_UART, GPP_UART_HANDLE, GPP_UART_IRQ, g_GPP_UART_TX_buffer,
			g_GPP_UART_RX_buffer,
			GPP_TX_BUF_LEN, GPP_RX_BUF_LEN);

	FSP_line.p_buffer = g_FSP_line_buffer;
	FSP_line.buffer_size = FSP_BUF_LEN;
	FSP_line.write_index = 0;

	s_pGPC_Sfp_Payload = (GPC_Sfp_Payload*) (&s_GPC_FspPacket.payload);
	s_pGPP_Sfp_Payload = (GPP_Sfp_Payload*) (&s_GPP_FspPacket.payload);

	if (FSP_line.buffer_size != 0) {
		memset((void*) FSP_line.p_buffer, 0, sizeof(FSP_line.p_buffer));
	}
	fsp_init(FSP_ADR_GPC);
}

/* :::::::::: CMD Line Task ::::::::::::: */

void FSP_Line_Task(void) {
	uint8_t time_out = 0;
//    fsp_packet_t  fsp_pkt;

	for (time_out = 50, escape = false;
			(!RX_BUFFER_EMPTY(&GPP_UART)) && (time_out != 0)
					&& (escape == false); time_out--) {
		FSP_line.RX_char = UART_Get_Char(&GPP_UART);
		escape = false;

		if (FSP_line.RX_char == FSP_PKT_SOD) {
			FSP_line.write_index = 0;
			is_receive_SOD = true;
		} else if ((FSP_line.RX_char == FSP_PKT_EOF)
				&& (is_receive_SOD == true)) {
			switch (frame_decode((uint8_t*) FSP_line.p_buffer,
					FSP_line.write_index, &s_GPP_FspPacket)) {
			//process command
			case FSP_PKT_NOT_READY:
				break;
			case FSP_PKT_READY:
				UART_Send_String(&RS232_UART, "Received FSP packet\r\n");
				FSP_PROCESS();
//					COPC_Process();
				break;
			case FSP_PKT_INVALID:

				break;
			case FSP_PKT_WRONG_ADR:
				UART_Send_String(&RS232_UART, "Wrong module adr\r\n");
//					usart1_send_string("Wrong module adr \r\n");
//					usart1_send_array((const char *)COPC_RX_Buff,COPC_RX_Index);
				break;
			case FSP_PKT_ERROR:
				UART_Send_String(&RS232_UART, "Packet error\r\n");
//					usart1_send_string("Packet error \r\n");
//					usart1_send_array((const char *)COPC_RX_Buff,COPC_RX_Index);
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
		} else {
			FSP_line.p_buffer[FSP_line.write_index] = FSP_line.RX_char;
			FSP_line.write_index++;

			if (FSP_line.write_index > FSP_line.buffer_size)
				FSP_line.write_index = 0;

		}
	}
}

void FSP_PROCESS() {

}
/* :::::::::: IRQ Handler ::::::::::::: */
void GPP_UART_IRQHandler(void) {
	if (LL_USART_IsActiveFlag_TXE(GPP_UART.handle) == true) {
		if (TX_BUFFER_EMPTY(&GPP_UART)) {
			// Buffer empty, so disable interrupts
			LL_USART_DisableIT_TXE(GPP_UART.handle);
		} else {
			// There is more data in the output buffer. Send the next byte
			UART_Prime_Transmit(&GPP_UART);
		}
	}

	if (LL_USART_IsActiveFlag_RXNE(GPP_UART.handle) == true) {
		GPP_UART.RX_irq_char = LL_USART_ReceiveData8(GPP_UART.handle);

		// NOTE: On win 10, default PUTTY when hit enter only send back '\r',
		// while on default HERCULES when hit enter send '\r\n' in that order.
		// The code bellow is modified so that it can work on PUTTY and HERCULES.
		if ((!RX_BUFFER_FULL(&GPP_UART))) {
//            if (GPP_UART.RX_irq_char == '\r')
//            {
//                GPP_UART.p_RX_buffer[GPP_UART.RX_write_index] = '\n';
//                ADVANCE_RX_WRITE_INDEX(&GPP_UART);
//            }
//            else
//            {
			GPP_UART.p_RX_buffer[GPP_UART.RX_write_index] =
					GPP_UART.RX_irq_char;
			ADVANCE_RX_WRITE_INDEX(&GPP_UART);
//            }
		}
	}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
