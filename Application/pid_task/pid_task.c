/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include <stdbool.h>

#include "app.h"

#include "stm32f0xx_ll_gpio.h"

#include "pid_task.h"

#include "pid.h"
#include "pwm.h"
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* :::::::::: PID Task State ::::::::::::: */
typedef enum
{
	PID_OFF_STATE,
    PID_CAP_CHARGE_STATE,
    PID_H_BRIDGE_STATE,
} PID_State_typedef;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
extern uart_stdio_typedef  RS232_UART;
static PID_State_typedef PID_State = PID_OFF_STATE;

static PWM_TypeDef 	Flyback_300V_Switching_PWM;
static PWM_TypeDef	Flyback_50V_Switching_PWM;

		bool		PID_is_300V_on = false;
		uint16_t 	PID_300V_set_voltage = 0;
		uint8_t 	PID_300V_PWM_duty;

	   	bool		PID_is_50V_on = false;
	   	uint16_t 	PID_50V_set_voltage	= 0;
		uint8_t 	PID_50V_PWM_duty;

static PID_TypeDef Charge_300V_Cap_PID =
{
	.PID_Mode 		= 	_PID_MODE_AUTOMATIC,
	.PON_Type 		= 	_PID_P_ON_E,
	.PID_Direction 	=	_PID_CD_DIRECT,
	.Kp				= 	(0.04 + 0.26),
	.Ki				= 	0.001,
	.Kd 			=	0.0,
	.MyInput		=	&g_Feedback_Voltage[0],
	.MyOutput		= 	&PID_300V_PWM_duty,
	.MySetpoint		=	&PID_300V_set_voltage,
	.Output_Min		= 	0,
	.Output_Max		=	30,
};

static PID_TypeDef Charge_50V_Cap_PID =
{
	.PID_Mode 		= 	_PID_MODE_AUTOMATIC,
	.PON_Type 		= 	_PID_P_ON_E,
	.PID_Direction 	=	_PID_CD_DIRECT,
	.Kp				= 	(0.04 + 0.16),
	.Ki				= 	0.001,
	.Kd 			=	0.0,
	.MyInput		=	&g_Feedback_Voltage[1],
	.MyOutput		= 	&PID_50V_PWM_duty,
	.MySetpoint		=	&PID_50V_set_voltage,
	.Output_Min		= 	0,
	.Output_Max		=	12,
};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: PID Init ::::::::::::: */
void PID_Task_Init(void)
{
	// Init 300V PWM PID
	PID_Init(&Charge_300V_Cap_PID);

	PWM_Init(	&Flyback_300V_Switching_PWM, FLYBACK_SW1_HANDLE, FLYBACK_SW1_CHANNEL,
				LL_TIM_OCMODE_PWM1, LL_TIM_OCPOLARITY_HIGH);

	PWM_Set_Freq(&Flyback_300V_Switching_PWM, 60000);
    PWM_Enable(&Flyback_300V_Switching_PWM);

	// Init 50V PWM PID
	PID_Init(&Charge_50V_Cap_PID);

	PWM_Init(	&Flyback_50V_Switching_PWM, FLYBACK_SW2_HANDLE, FLYBACK_SW2_CHANNEL,
				LL_TIM_OCMODE_PWM1, LL_TIM_OCPOLARITY_HIGH);

	PWM_Set_Freq(&Flyback_50V_Switching_PWM, 60000);
    PWM_Enable(&Flyback_50V_Switching_PWM);

	LL_GPIO_SetOutputPin(FLYBACK_SD1_PORT, FLYBACK_SD1_PIN);
	LL_GPIO_SetOutputPin(FLYBACK_SD2_PORT, FLYBACK_SD2_PIN);
}

/* :::::::::: PID Task ::::::::::::: */
//TODO: Adaptive PID, Ki, or sync uart command to change Ki.
void PID_Task(void*)
{
	if (PID_is_300V_on == false)
	{
		PWM_Set_Duty(&Flyback_300V_Switching_PWM, 0);
	}
	else if (PID_is_300V_on == true)
	{
		PID_Compute(&Charge_300V_Cap_PID);
		PWM_Set_Duty(&Flyback_300V_Switching_PWM, PID_300V_PWM_duty);
	}

	if (PID_is_50V_on == false)
	{
		PWM_Set_Duty(&Flyback_50V_Switching_PWM, 0);
	}
	else if (PID_is_50V_on == true)
	{
		PID_Compute(&Charge_50V_Cap_PID);
		PWM_Set_Duty(&Flyback_50V_Switching_PWM, PID_50V_PWM_duty);
	}
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
