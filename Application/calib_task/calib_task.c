/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stdbool.h>

#include "stm32f0xx_ll_gpio.h"

#include "app.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* :::::::::: Calib Task State ::::::::::::: */
typedef enum
{
	CALIB_OFF_STATE,
    CALIB_START_STATE,
    CALIB_PROCESS_STATE,
} Calib_State_typedef;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static Calib_State_typedef Calib_State = CALIB_OFF_STATE;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static inline void charge_cap_procedure(uint16_t hv_set_volt, uint16_t lv_set_volt);
static inline void stop_cap_procedure();
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
extern uart_stdio_typedef  RS232_UART;
extern uart_stdio_typedef  RF_UART;

struct _hv_calib_coefficient hv_calib_coefficient = 
{
    .default_value  = 8.44,
    .at_50v         = 0.0,
    .at_100v        = 0.0,
    .at_200v        = 0.0,
    .at_300v        = 0.0,
    .average_value  = 8.44,
};

struct _lv_calib_coefficient lv_calib_coefficient =
{
    .default_value  = 55.6,
    .at_5v          = 0.0,
    .at_10v         = 0.0,
    .at_25v         = 0.0,
    .at_50v         = 0.0,
    .average_value  = 55.6,
};

uint16_t g_HV_Calib_set = 0;
uint16_t g_LV_Calib_set = 0;

uint32_t g_HV_Measure_mv = 0;
uint32_t g_LV_Measure_mv = 0;

uint8_t calib_state_count = 0;

bool g_is_calib_running     = false;
bool g_is_measure_available = false;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: Discharge Task Init ::::::::::::: */
void Calib_Task_Init(void)
{
	;
}

/* :::::::::: Discharge Task ::::::::::::: */
void Calib_Task(void*)
{
    switch (Calib_State)
    {
    case CALIB_OFF_STATE:
        if (g_is_calib_running == true)
        {
            calib_state_count = 0;
            g_is_measure_available = 0;
            Calib_State = CALIB_START_STATE;
        }
        else
        {
            SchedulerTaskDisable(5);
        }
        
        break;
    case CALIB_START_STATE:
        if (g_is_calib_running == false)
        {
            stop_cap_procedure();
            g_is_Discharge_300V_On  = 1;
            g_is_Discharge_50V_On   = 1;

            Calib_State = CALIB_OFF_STATE;
        }

        switch (calib_state_count)
        {
        case 0:
            UART_Send_String(&RS232_UART, "CALIB HV CAP AT 50V AND LV CAP AT 5V\n");
            UART_Send_String(&RF_UART, "CALIB HV CAP AT 50V AND LV CAP AT 5V\n");

            UART_Send_String(&RS232_UART, "> ");
            UART_Send_String(&RF_UART, "> ");

            charge_cap_procedure(50, 5);
            Calib_State = CALIB_PROCESS_STATE;
            break;
        case 1:
            UART_Send_String(&RS232_UART, "CALIB HV CAP AT 100V AND LV CAP AT 10V\n");
            UART_Send_String(&RF_UART, "CALIB HV CAP AT 100V AND LV CAP AT 10V\n");

            UART_Send_String(&RS232_UART, "> ");
            UART_Send_String(&RF_UART, "> ");

            charge_cap_procedure(100, 10);
            Calib_State = CALIB_PROCESS_STATE;
            break;
        case 2:
            UART_Send_String(&RS232_UART, "CALIB HV CAP AT 200V AND LV CAP AT 25V\n");
            UART_Send_String(&RF_UART, "CALIB HV CAP AT 200V AND LV CAP AT 25V\n");

            UART_Send_String(&RS232_UART, "> ");
            UART_Send_String(&RF_UART, "> ");

            charge_cap_procedure(200, 25);
            Calib_State = CALIB_PROCESS_STATE;
            break;
        case 3:
            UART_Send_String(&RS232_UART, "CALIB HV CAP AT 300V AND LV CAP AT 50V\n");
            UART_Send_String(&RF_UART, "CALIB HV CAP AT 300V AND LV CAP AT 50V\n");

            UART_Send_String(&RS232_UART, "> ");
            UART_Send_String(&RF_UART, "> ");

            charge_cap_procedure(300, 50);
            Calib_State = CALIB_PROCESS_STATE;
            break;

        default:
            break;
        }
            break;
    case CALIB_PROCESS_STATE:
        if (g_is_calib_running == false)
        {
            stop_cap_procedure();
            g_is_Discharge_300V_On  = 1;
            g_is_Discharge_50V_On   = 1;

            Calib_State = CALIB_OFF_STATE;
        }
        else if (g_is_measure_available == true)
        {
            switch (calib_state_count)
            {
            case 0:
                hv_calib_coefficient.at_50v = 1000.0 * ((float)g_Feedback_Voltage[0] / (float)g_HV_Measure_mv);
                lv_calib_coefficient.at_5v  = 1000.0 * ((float)g_Feedback_Voltage[1] / (float)g_LV_Measure_mv);

                stop_cap_procedure();
                calib_state_count = 1;
                g_is_measure_available = false;
                Calib_State = CALIB_START_STATE;
                break;
            case 1:
                hv_calib_coefficient.at_100v = 1000.0 * ((float)g_Feedback_Voltage[0] / (float)g_HV_Measure_mv);
                lv_calib_coefficient.at_10v  = 1000.0 * ((float)g_Feedback_Voltage[1] / (float)g_LV_Measure_mv);

                stop_cap_procedure();
                calib_state_count = 2;
                g_is_measure_available = false;
                Calib_State = CALIB_START_STATE;
                break;
            case 2:
                hv_calib_coefficient.at_200v = 1000.0 * ((float)g_Feedback_Voltage[0] / (float)g_HV_Measure_mv);
                lv_calib_coefficient.at_25v  = 1000.0 * ((float)g_Feedback_Voltage[1] / (float)g_LV_Measure_mv);

                stop_cap_procedure();
                calib_state_count = 3;
                g_is_measure_available = false;
                Calib_State = CALIB_START_STATE;
                break;
            case 3:
                hv_calib_coefficient.at_300v = 1000.0 * ((float)g_Feedback_Voltage[0] / (float)g_HV_Measure_mv);
                lv_calib_coefficient.at_50v  = 1000.0 * ((float)g_Feedback_Voltage[1] / (float)g_LV_Measure_mv);

                hv_calib_coefficient.average_value = 
                (hv_calib_coefficient.at_50v + hv_calib_coefficient.at_100v + hv_calib_coefficient.at_200v + hv_calib_coefficient.at_300v) / 4.0;

                lv_calib_coefficient.average_value = 
                (lv_calib_coefficient.at_5v + lv_calib_coefficient.at_10v + lv_calib_coefficient.at_25v + lv_calib_coefficient.at_50v) / 4.0;

                UART_Send_String(&RS232_UART, "FINNISH RUNNING CALIB, CAP IS DISCHARGING\n");
                UART_Send_String(&RF_UART, "FINNISH RUNNING CALIB, CAP IS DISCHARGING\n");

                UART_Send_String(&RS232_UART, "> ");
                UART_Send_String(&RF_UART, "> ");

                stop_cap_procedure();
                g_is_Discharge_300V_On  = 1;
                g_is_Discharge_50V_On   = 1;

                calib_state_count = 0;
                g_is_measure_available = false;
                g_is_calib_running = false;
                Calib_State = CALIB_OFF_STATE;
                SchedulerTaskDisable(5);
                break;

            default:
                break;
            }
        }
        break;

    default:
        break;
    }
}

void Calib_Calculate(uint16_t hv_set_voltage, uint16_t lv_set_voltage)
{
    PID_300V_set_voltage    = hv_set_voltage * hv_calib_coefficient.average_value;
    PID_50V_set_voltage     = lv_set_voltage * lv_calib_coefficient.average_value;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static inline void charge_cap_procedure(uint16_t hv_set_volt, uint16_t lv_set_volt)
{
    PID_300V_set_voltage    = hv_set_volt * hv_calib_coefficient.default_value;
    PID_50V_set_voltage     = lv_set_volt * lv_calib_coefficient.default_value;

    g_is_Discharge_300V_On = 0;
    g_is_Discharge_50V_On = 0;

    PID_is_300V_on  = 1;
    PID_is_50V_on   = 1;
}

static inline void stop_cap_procedure()
{
    PID_is_300V_on  = 0;
    PID_is_50V_on   = 0;
}
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
