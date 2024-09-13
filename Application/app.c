// APP HEADER //
#include "app.h"

uart_stdio_typedef H_BRIDGE_UART; 

//TODO: Create a system to handle hard fault or smth like that.

#define         SCHEDULER_TASK_COUNT  3
uint32_t 		g_ui32SchedulerNumTasks = SCHEDULER_TASK_COUNT;
tSchedulerTask 	g_psSchedulerTable[SCHEDULER_TASK_COUNT] =
                {
                    {
                            &ADC_Task,
                            (void *) 0,
                            62,                         //call every 125us
                            0,			                //count from start
                            true		                //is active
                    },
                    {
                            &PID_Task,
                            (void *) 0,
                            125,                        //call every 1ms
                            0,			                //count from start
                            true		                //is active
                    },
                    {
                            &CMD_Line_Task,
                            (void *) 0,
                            125,                        //call every 1ms
                            0,                          //count from start
                            true                        //is active
                    },
                };

void App_Main(void)
{   
    // STM32F030CCT6 @ 36MHz, 
    // can run scheduler tick max @ 4us.
    SchedulerInit(250000);

    ADC_Task_Init(LL_ADC_SAMPLINGTIME_7CYCLES_5);
    PID_Task_Init();
    CMD_Line_Task_Init(64);

    UART_Init(&H_BRIDGE_UART, H_BRIDGE_UART_HANDLE, H_BRIDGE_UART_IRQ, 64, 64);

    while (1)
    {
        SchedulerRun();
    }
}

void H_BRIDGE_TX_IRQHandler(void)
{
    if(LL_USART_IsActiveFlag_TXE(H_BRIDGE_UART.handle) == true)
    {
        if(TX_BUFFER_EMPTY(&H_BRIDGE_UART))
        {
            // Buffer empty, so disable interrupts
            LL_USART_DisableIT_TXE(H_BRIDGE_UART.handle);
        }
        else
        {
            // There is more data in the output buffer. Send the next byte
            UART_Prime_Transmit(&H_BRIDGE_UART);
        }
    }
}

void H_BRIDGE_RX_IRQHandler(void)
{
    if(LL_USART_IsActiveFlag_RXNE(H_BRIDGE_UART.handle) == true)
    {
        if(!RX_BUFFER_FULL(&H_BRIDGE_UART))
        {
        	H_BRIDGE_UART.RX_irq_char = LL_USART_ReceiveData8(H_BRIDGE_UART.handle);
            ADVANCE_RX_WRITE_INDEX(&H_BRIDGE_UART);
        }
    }
}
