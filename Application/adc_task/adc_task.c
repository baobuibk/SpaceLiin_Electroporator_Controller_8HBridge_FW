/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "adc_task.h"

#include "app.h"

#include "pwm.h"
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
static uint8_t  ADC_channel_index = 0;
static uint16_t ADC_Value[ADC_CHANNEL_COUNT] = {0};

static bool is_ADC_read_completed       = false;
//static bool is_ADC_sequence_completed   = false;

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
extern PWM_TypeDef 	Flyback_300V_Switching_PWM;
uint16_t g_Feedback_Voltage[ADC_CHANNEL_COUNT] = {0};

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: ADC Task Init :::::::: */
void ADC_Task_Init(uint32_t Sampling_Time)
{
    ADC_Init(ADC_FEEDBACK_HANDLE, Sampling_Time);

    LL_ADC_REG_SetSequencerChannels(ADC_FEEDBACK_HANDLE, ADC_300V_CHANNEL | ADC_50V_CHANNEL);
    LL_ADC_REG_SetSequencerDiscont(ADC_FEEDBACK_HANDLE, LL_ADC_REG_SEQ_DISCONT_1RANK);

    LL_ADC_EnableIT_EOC(ADC_FEEDBACK_HANDLE);
    //LL_ADC_EnableIT_EOS(ADC_FEEDBACK_HANDLE);

    LL_ADC_REG_StartConversion(ADC_FEEDBACK_HANDLE);
}

/* :::::::::: ADC Task ::::::::::::: */
//TODO: Áp dụng ring buffer và mạch lọc cho ADC.
void ADC_Task(void*)
{
    if (is_ADC_read_completed == true)
    {
        is_ADC_read_completed = false;

        LL_ADC_REG_StartConversion(ADC_FEEDBACK_HANDLE);
    }
    /*
    else if (is_ADC_sequence_completed == true)
    {
        is_ADC_sequence_completed = false;
        ADC_channel_index = 0;

        LL_ADC_REG_StartConversion(ADC_FEEDBACK_HANDLE);
    }
    */
}

/* :::::::::: ADC Interupt Handler ::::::::::::: */
void ADC_Task_IRQHandler(void)
{
    if(LL_ADC_IsActiveFlag_EOC(ADC_FEEDBACK_HANDLE) == true)
    {
        is_ADC_read_completed = true;
        LL_ADC_ClearFlag_EOC(ADC_FEEDBACK_HANDLE);

        ADC_Value[ADC_channel_index] = LL_ADC_REG_ReadConversionData12(ADC_FEEDBACK_HANDLE);
        g_Feedback_Voltage[ADC_channel_index] = 
            __LL_ADC_CALC_DATA_TO_VOLTAGE(3300, ADC_Value[ADC_channel_index], LL_ADC_RESOLUTION_12B);

        /*
        if((ADC_channel_index == 0) && (PID_is_300V_on == true))
        {
            if(g_Feedback_Voltage[0] < PID_300V_set_voltage)
            {
                if (PID_300V_PWM_duty < 10)
                    PID_300V_PWM_duty++;
            }
            else
            {
                if (PID_300V_PWM_duty > 0)
                    PID_300V_PWM_duty--;
            }

            Flyback_300V_Switching_PWM.Duty = (Flyback_300V_Switching_PWM.Freq * (PID_300V_PWM_duty / 100.0));
            LL_TIM_OC_SetCompareCH1(Flyback_300V_Switching_PWM.TIMx, Flyback_300V_Switching_PWM.Duty);
        }
        */

        if(ADC_channel_index >= 1)
        {
            ADC_channel_index = 0;
        }
        else
        {
            ADC_channel_index = ADC_channel_index + 1;
        }  
    }
/*
    else if(LL_ADC_IsActiveFlag_EOS(ADC_FEEDBACK_HANDLE) == true)
    {
        is_ADC_sequence_completed = true;
        LL_ADC_ClearFlag_EOS(ADC_FEEDBACK_HANDLE);
    }
*/
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
