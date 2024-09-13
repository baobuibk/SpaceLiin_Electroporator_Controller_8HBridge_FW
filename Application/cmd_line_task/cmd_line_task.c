/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <string.h>
#include <stdlib.h>

#include "cmd_line_task.h"

#include "app.h"

#include "cmd_line.h"
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
struct _cmd_line_typedef
{
                uint16_t    buffer_size;
                char*       p_buffer;

    volatile    uint16_t    write_index;
    volatile    uint16_t    read_index;
    volatile    char        RX_char;
};
typedef struct _cmd_line_typedef cmd_line_typedef;
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
uart_stdio_typedef  RS232_UART;
uart_stdio_typedef  RF_UART;

cmd_line_typedef    CMD_line;

static const char * ErrorCode[5] = 
{
    "OK\r\n",
    "CMDLINE_BAD_CMD\r\n",
    "CMDLINE_TOO_MANY_ARGS\r\n",
    "CMDLINE_TOO_FEW_ARGS\r\n",
    "CMDLINE_INVALID_ARG\r\n" 
};

const char SPLASH[][65] = 
{
{"\r\n"},
{".........................................................\r\n"},
{".........................................................\r\n"},
{"..    ____                       _     _               ..\r\n"},
{"..   / ___| _ __   __ _  ___ ___| |   (_)_ __  _ __    ..\r\n"},
{"..   \\___ \\| '_ \\ / _` |/ __/ _ \\ |   | | '_ \\| '_ \\   ..\r\n"},
{"..    ___) | |_) | (_| | (_|  __/ |___| | | | | | | |  ..\r\n"},
{"..   |____/| .__/ \\__,_|\\___\\___|_____|_|_| |_|_| |_|  ..\r\n"},
{"..         |_|    _   _ _____ _____                    ..\r\n"},
{"..               | | | | ____| ____|                   ..\r\n"},
{"..               | |_| |  _| |  _|                     ..\r\n"},
{"..               |  _  | |___| |___                    ..\r\n"},
{"..               |_| |_|_____|_____|                   ..\r\n"},
{"..            __     _____   ___   ___                 ..\r\n"},
{"..            \\ \\   / / _ \\ / _ \\ / _ \\                ..\r\n"},
{"..             \\ \\ / / | | | | | | | | |               ..\r\n"},
{"..              \\ V /| |_| | |_| | |_| |               ..\r\n"},
{"..               \\_/  \\___(_)___(_)___/                ..\r\n"},
{".........................................................\r\n"},
{".........................................................\r\n"},                                                   
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static uint8_t      is_buffer_full(volatile uint16_t *pui16Read,
                            volatile uint16_t *pui16Write, uint16_t ui16Size);

static uint8_t      is_buffer_empty(volatile uint16_t *pui16Read,
                            volatile uint16_t *pui16Write);

static uint16_t     get_buffer_count(volatile uint16_t *pui16Read,
                            volatile uint16_t *pui16Write, uint16_t ui16Size);

static uint16_t     advance_buffer_index(volatile uint16_t* pui16Index, uint16_t ui16Size);

static uint16_t     retreat_buffer_index(volatile uint16_t* pui16Index, uint16_t ui16Size);

static void         CMD_send_splash(uart_stdio_typedef* p_uart);

static void         CMD_line_test(void);

//*****************************************************************************
//
// Macros to determine number of free and used bytes in the receive buffer.
//
//*****************************************************************************
#define CMD_BUFFER_SIZE(p_cmd_line)          ((p_cmd_line)->buffer_size)

#define CMD_BUFFER_USED(p_cmd_line)          (get_buffer_count(&(p_cmd_line)->read_index,  \
                                                                  &(p_cmd_line)->write_index, \
                                                                  (p_cmd_line)->buffer_size))

#define CMD_BUFFER_FREE(p_cmd_line)          (CMD_BUFFER_SIZE - CMD_BUFFER_USED(p_cmd_line))

#define CMD_BUFFER_EMPTY(p_cmd_line)         (is_buffer_empty(&(p_cmd_line)->read_index,   \
                                                                  &(p_cmd_line)->write_index))

#define CMD_BUFFER_FULL(p_cmd_line)          (is_buffer_full(&(p_cmd_line)->read_index,  \
                                                                 &(p_cmd_line)->write_index, \
                                                                 (p_cmd_line)->buffer_size))

#define ADVANCE_CMD_WRITE_INDEX(p_cmd_line)  (advance_buffer_index(&(p_cmd_line)->write_index, \
                                                                      (p_cmd_line)->buffer_size))

#define ADVANCE_CMD_READ_INDEX(p_cmd_line)   (advance_buffer_index(&(p_cmd_line)->read_index, \
                                                                      (p_cmd_line)->buffer_size))

#define RETREAT_CMD_WRITE_INDEX(p_cmd_line)  (retreat_buffer_index(&(p_cmd_line)->write_index, \
                                                                      (p_cmd_line)->buffer_size))

#define RETREAT_CMD_READ_INDEX(p_cmd_line)   (retreat_buffer_index(&(p_cmd_line)->read_index, \
                                                                      (p_cmd_line)->buffer_size))


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
tCmdLineEntry g_psCmdTable[] =
{
    { "MARCO", CMD_line_test, "TEST" },
	{0,0,0}
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: CMD Line Task Init :::::::: */
void CMD_Line_Task_Init(uint16_t _buffer_size)
{
    UART_Init(&RS232_UART, RS232_UART_HANDLE, RS232_UART_IRQ, 64, 64);
    //UART_Init(&RF_UART, RF_UART_HANDLE, RF_UART_IRQ, 64, 64);

    if(_buffer_size != 0)
    {
        CMD_line.p_buffer = (char *)malloc(CMD_line.buffer_size * sizeof(char));
        memset((void *)CMD_line.p_buffer, 0, sizeof(CMD_line.p_buffer));
    }
    
    CMD_line.write_index 	= 0;
    CMD_line.read_index		= 0;

    UART_Write(&RS232_UART, "GPC FIRMWARE V0.0.0 \r\n", 22);
    UART_Write(&RS232_UART, ">", 1);
    CMD_send_splash(&RS232_UART);
}

/* :::::::::: CMD Line Task ::::::::::::: */
void CMD_Line_Task(void)
{
    uint8_t return_value;

    while((!RX_BUFFER_EMPTY(&RS232_UART)) && (!CMD_BUFFER_FULL(&CMD_line)))
    {
        CMD_line.RX_char = UART_Get_Char(&RS232_UART);
        UART_Write(&RS232_UART, &CMD_line.RX_char, 1);

        if((CMD_line.RX_char == '\r') || (CMD_line.RX_char == '\n'))
        {
            if(!CMD_BUFFER_EMPTY(&CMD_line))
            {
                CMD_line.p_buffer[CMD_line.write_index] = 0;
                ADVANCE_CMD_WRITE_INDEX(&CMD_line);

                return_value = CmdLineProcess(CMD_line.p_buffer);
                CMD_line.read_index = CMD_line.write_index;

                UART_Printf(&RS232_UART, ErrorCode[return_value]);
                UART_Write(&RS232_UART, "> ", 2);
            }
            else 
                UART_Write(&RS232_UART, "\r\n> ", 4);
        }
        else if((CMD_line.RX_char == 8) || (CMD_line.RX_char == 127))
        {
            if(!CMD_BUFFER_EMPTY(&CMD_line))
                RETREAT_CMD_WRITE_INDEX(&CMD_line);
        }
        else
        {
            CMD_line.p_buffer[CMD_line.write_index] = CMD_line.RX_char;
            ADVANCE_CMD_WRITE_INDEX(&CMD_line);
        }
    }
}

void CMD_line_test(void)
{
    UART_Write(&RS232_UART, "POLO\r", 5);
}

/* :::::::::: IRQ Handler ::::::::::::: */
void RS232_TX_IRQHandler(void)
{
    if(LL_USART_IsActiveFlag_TXE(RS232_UART.handle) == true)
    {
        if(TX_BUFFER_EMPTY(&RS232_UART))
        {
            // Buffer empty, so disable interrupts
            LL_USART_DisableIT_TXE(RS232_UART.handle);
        }
        else
        {
            // There is more data in the output buffer. Send the next byte
            UART_Prime_Transmit(&RS232_UART);
        }
    }
}

void RF_TX_IRQHandler(void)
{   
    if(LL_USART_IsActiveFlag_TXE(RF_UART.handle) == true)
    {
        if(TX_BUFFER_EMPTY(&RF_UART))
        {
            // Buffer empty, so disable interrupts
            LL_USART_DisableIT_TXE(RF_UART.handle);
        }
        else
        {
            // There is more data in the output buffer. Send the next byte
            UART_Prime_Transmit(&RF_UART);
        }
    }
}

void RS232_RX_IRQHandler(void)
{
    if(LL_USART_IsActiveFlag_RXNE(RS232_UART.handle) == true)
    {
        if(!RX_BUFFER_FULL(&RS232_UART))
        {
            RS232_UART.RX_irq_char = LL_USART_ReceiveData8(RS232_UART.handle);
            ADVANCE_RX_WRITE_INDEX(&RS232_UART);
        }
    }
}

void RF_RX_IRQHandler(void)
{
    if(LL_USART_IsActiveFlag_RXNE(RF_UART.handle) == true)
    {
        if(!RX_BUFFER_FULL(&RF_UART))
        {
            RF_UART.RX_irq_char = LL_USART_ReceiveData8(RF_UART.handle);
            ADVANCE_RX_WRITE_INDEX(&RF_UART);
        }
    }
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
//*****************************************************************************
//
//! Determines whether the ring buffer whose pointers and size are provided
//! is full or not.
//!
//! \param pui16Read points to the read index for the buffer.
//! \param pui16Write points to the write index for the buffer.
//! \param ui16Size is the size of the buffer in bytes.
//!
//! This function is used to determine whether or not a given ring buffer is
//! full.  The structure of the code is specifically to ensure that we do not
//! see warnings from the compiler related to the order of volatile accesses
//! being undefined.
//!
//! \return Returns \b 1 if the buffer is full or \b 0 otherwise.
//
//*****************************************************************************

static uint8_t is_buffer_full(volatile uint16_t *pui16Read,
             volatile uint16_t *pui16Write, uint16_t ui16Size)
{
    uint16_t ui16Write;
    uint16_t ui16Read;

    ui16Write = *pui16Write;
    ui16Read = *pui16Read;

    return(advance_buffer_index(&ui16Write, ui16Size) == (ui16Read - 1)) ? 1 : 0;
}


//*****************************************************************************
//
//! Determines whether the ring buffer whose pointers and size are provided
//! is empty or not.
//!
//! \param pui16Read points to the read index for the buffer.
//! \param pui16Write points to the write index for the buffer.
//!
//! This function is used to determine whether or not a given ring buffer is
//! empty.  The structure of the code is specifically to ensure that we do not
//! see warnings from the compiler related to the order of volatile accesses
//! being undefined.
//!
//! \return Returns \b 1 if the buffer is empty or \b 0 otherwise.
//
//*****************************************************************************

static uint8_t is_buffer_empty(volatile uint16_t *pui16Read,
              volatile uint16_t *pui16Write)
{
    uint16_t ui16Write;
    uint16_t ui16Read;

    ui16Write = *pui16Write;
    ui16Read = *pui16Read;

    return((ui16Read == ui16Write) ? 1 : 0);
}


//*****************************************************************************
//
//! Determines the number of bytes of data contained in a ring buffer.
//!
//! \param pui16Read points to the read index for the buffer.
//! \param pui16Write points to the write index for the buffer.
//! \param ui16Size is the size of the buffer in bytes.
//!
//! This function is used to determine how many bytes of data a given ring
//! buffer currently contains.  The structure of the code is specifically to
//! ensure that we do not see warnings from the compiler related to the order
//! of volatile accesses being undefined.
//!
//! \return Returns the number of bytes of data currently in the buffer.
//
//*****************************************************************************

static uint16_t get_buffer_count(volatile uint16_t *pui16Read,
               volatile uint16_t *pui16Write, uint16_t ui16Size)
{
    uint16_t ui16Write;
    uint16_t ui16Read;

    ui16Write = *pui16Write;
    ui16Read = *pui16Read;

    return((ui16Write >= ui16Read) ? (ui16Write - ui16Read) :
           (ui16Size - (ui16Read - ui16Write)));
}

//*****************************************************************************
//
//! Adding +1 to the index
//!
//! \param pui16Read points to the read index for the buffer.
//! \param pui16Write points to the write index for the buffer.
//! \param ui16Size is the size of the buffer in bytes.
//!
//! This function is use to advance the index by 1, if the index
//! already hit the uart size then it will reset back to 0.
//!
//! \return Returns the number of bytes of data currently in the buffer.
//
//*****************************************************************************

static uint16_t advance_buffer_index(volatile uint16_t* pui16Index, uint16_t ui16Size)
{

    ((*pui16Index) == ui16Size) ? ((*pui16Index) = 0) : ((*pui16Index) += 1);

    return(*pui16Index);
}

static uint16_t retreat_buffer_index(volatile uint16_t* pui16Index, uint16_t ui16Size)
{
    ((*pui16Index) == 0) ? ((*pui16Index) = ui16Size) : ((*pui16Index) -= 1);

    return(*pui16Index);
}

static void CMD_send_splash(uart_stdio_typedef* p_uart)
{
    for(uint8_t i = 0 ; i < 21 ; i++) {
		UART_Write(p_uart, &SPLASH[i][0], 65);
	}
	UART_Write(p_uart, ">", 1);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
