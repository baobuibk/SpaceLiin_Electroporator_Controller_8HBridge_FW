// APP HEADER //
#include "app.h"

//TODO: Create a system to handle hard fault or smth like that.
//extern uart_stdio_typedef  RS232_UART;
//static int volt_log(void*);

#define         SCHEDULER_TASK_COUNT  4
uint32_t        g_ui32SchedulerNumTasks = SCHEDULER_TASK_COUNT;
tSchedulerTask 	g_psSchedulerTable[SCHEDULER_TASK_COUNT] =
                {
                    {
                            &ADC_Task,
                            (void *) 0,
                            5,                 //call every 248us
                            0,			       //count from start
                            true		       //is active
                    },
                    {
                            &PID_Task,
                            (void *) 0,
                            10,               //call every 500us
                            0,			      //count from start
                            true		      //is active
                    },
                    {
                            &Discharge_Task,
                            (void *) 0,
                            15,               //call every 500us
                            0,			      //count from start
                            true		      //is active
                    },
                    {
                            &CMD_Line_Task,
                            (void *) 0,
                            20,               //call every 500us
                            0,                //count from start
                            true              //is active
                    },
                };

void App_Main(void)
{   
    // STM32F030CCT6 @ 36MHz, 
    // can run scheduler tick max @ 100us.
    SchedulerInit(10000);

    ADC_Task_Init(LL_ADC_SAMPLINGTIME_7CYCLES_5);
    PID_Task_Init();
    Discharge_Task_Init();
    CMD_Line_Task_Init();

    while (1)
    {
        SchedulerRun();
    }
}

/*
static int volt_log(void*)
{
    if ((PID_is_50V_on == true) || (PID_is_300V_on == true))
	{
		UART_Printf(&RS232_UART, "300V cap: %d; 50V cap: %d\n", g_Feedback_Voltage[0], g_Feedback_Voltage[1]);
        UART_Printf(&RS232_UART, "300V duty: %d; 50V duty: %d\n", PID_300V_PWM_duty, PID_50V_PWM_duty);
	}
}
*/
