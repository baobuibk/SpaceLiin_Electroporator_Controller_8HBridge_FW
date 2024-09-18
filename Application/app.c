// APP HEADER //
#include "app.h"

//TODO: Create a system to handle hard fault or smth like that.

#define         SCHEDULER_TASK_COUNT  4
uint32_t 		g_ui32SchedulerNumTasks = SCHEDULER_TASK_COUNT;
tSchedulerTask 	g_psSchedulerTable[SCHEDULER_TASK_COUNT] =
                {
                    {
                            &ADC_Task,
                            (void *) 0,
                            3,                         //call every 248us
                            0,			                //count from start
                            true		                //is active
                    },
                    {
                            &PID_Task,
                            (void *) 0,
                            5,                        //call every 500us
                            0,			                //count from start
                            true		                //is active
                    },
                    {
                            &Discharge_Task,
                            (void *) 0,
                            5,                        //call every 500us
                            0,			                //count from start
                            true		                //is active
                    },
                    {
                            &CMD_Line_Task,
                            (void *) 0,
                            5,                        //call every 500us
                            0,                          //count from start
                            true                        //is active
                    },
                };

void App_Main(void)
{   
    // STM32F030CCT6 @ 36MHz, 
    // can run scheduler tick max @ 4us.
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
