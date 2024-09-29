/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Include~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdbool.h>
#include <stdlib.h>

#include "cmd_line_task.h"
#include "command.h"

#include "app.h"

#include "cmd_line.h"
#include "fsp.h"
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Private Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Private Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
extern uart_stdio_typedef  RS232_UART;
extern uart_stdio_typedef  GPP_UART;

tCmdLineEntry g_psCmdTable[] =
{
    { "MARCO",          CMD_line_test,      "TEST" },
    { "CAP_VOLT",       CMD_CAP_VOLT,       "Set cap voltage"},
    { "CAP_CONTROL",    CMD_CAP_CONTROL,    "Control charger on/off"},
    { "CAP_RELEASE",    CMD_CAP_RELEASE,    "Control releasing cap"},
    { "PULSE_COUNT",    CMD_PULSE_COUNT,    "Set number of pulse" },
    { "PULSE_DELAY",    CMD_PULSE_DELAY,    "Set delay between pulse hv and lv"},
    { "PULSE_HV",       CMD_PULSE_HV,       "Set hs pulse on time and off time" },
    { "PULSE_LV",       CMD_PULSE_LV,       "Set ls pulse on time and off time" },
    { "PULSE_CONTROL",  CMD_PULSE_CONTROL,  "Start pulsing" },
    { "RELAY_SET",      CMD_RELAY_SET,      "Set up cuvette"},
    { "RELAY_CONTROL",  CMD_RELAY_CONTROL,  "Stop cuvette"},
    { "CHANNEL_SET",    CMD_CHANNEL_SET,    "Choose a cap channel"},
    { "CHANNEL_CONTROL",CMD_CHANNEL_CONTROL,"Control the setted channel"},
	{0,0,0}
};
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
int CMD_line_test(int argc, char *argv[])
{
    UART_Send_String(&RS232_UART, "> POLO\n");
    return CMDLINE_OK;
}

int CMD_CAP_VOLT(int argc, char *argv[])
{
    if (argc < 3) 
        return CMDLINE_TOO_FEW_ARGS;
    else if (argc > 3)
        return CMDLINE_TOO_MANY_ARGS;

    int receive_argm[2];

    receive_argm[0] = atoi(argv[1]);
    receive_argm[1] = atoi(argv[2]);

    if ((receive_argm[0] > 300) || (receive_argm[0] < 0))
        return CMDLINE_INVALID_ARG;
    else if ((receive_argm[1] > 50) || (receive_argm[1] < 0))
        return CMDLINE_INVALID_ARG;

    //PID_300V_set_voltage    = (receive_argm[0] * 10) - 453;
    PID_300V_set_voltage    = (receive_argm[0] * 8.44);
    PID_50V_set_voltage     = (receive_argm[1] * 60) - 219;

    return CMDLINE_OK;
}

int CMD_CAP_CONTROL(int argc, char *argv[])
{
    if (argc < 3) 
        return CMDLINE_TOO_FEW_ARGS;
    else if (argc > 3)
        return CMDLINE_TOO_MANY_ARGS;

    int receive_argm[2];

    receive_argm[0] = atoi(argv[1]);
    receive_argm[1] = atoi(argv[2]);

    if ((receive_argm[0] > 1) || (receive_argm[0] < 0))
        return CMDLINE_INVALID_ARG;
    else if ((receive_argm[1] > 1) || (receive_argm[1] < 0))
        return CMDLINE_INVALID_ARG;

    if (receive_argm[0] == 1)
    {
        g_is_Discharge_300V_On = 0;
    }

    if (receive_argm[1] == 1)
    {
        g_is_Discharge_50V_On = 0;
    }

    PID_is_300V_on  = receive_argm[0];
    PID_is_50V_on   = receive_argm[1];

    return CMDLINE_OK;
}

int CMD_CAP_RELEASE(int argc, char *argv[])
{
    if (argc < 3) 
        return CMDLINE_TOO_FEW_ARGS;
    else if (argc > 3)
        return CMDLINE_TOO_MANY_ARGS;

    int receive_argm[2];

    receive_argm[0] = atoi(argv[1]);
    receive_argm[1] = atoi(argv[2]);

    if ((receive_argm[0] > 1) || (receive_argm[0] < 0))
        return CMDLINE_INVALID_ARG;
    else if ((receive_argm[1] > 1) || (receive_argm[1] < 0))
        return CMDLINE_INVALID_ARG;

    if (receive_argm[0] == 1)
    {
        PID_is_300V_on = 0;
    }

    if (receive_argm[1] == 1)
    {
        PID_is_50V_on = 0;
    }
    
    g_is_Discharge_300V_On  = receive_argm[0];
    g_is_Discharge_50V_On   = receive_argm[1];

    return CMDLINE_OK;
}

int CMD_PULSE_COUNT(int argc, char *argv[])
{
    if (argc < 3)
        return CMDLINE_TOO_FEW_ARGS;
    else if (argc > 3)
        return CMDLINE_TOO_MANY_ARGS;

    int receive_argm[2];

    receive_argm[0] = atoi(argv[1]);
    receive_argm[1] = atoi(argv[2]);

    if ((receive_argm[0] > 20) || (receive_argm[1] > 20))
        return CMDLINE_INVALID_ARG;

    uint8_t  cmd  = FSP_CMD_PULSE_COUNT;
    uint8_t  payload[2];
    payload[0]  =  receive_argm[0];
    payload[1]  =  receive_argm[1];
    fsp_packet_t  fsp_pkt;
    fsp_gen_cmd_w_data_pkt(cmd,  payload,  sizeof(payload), FSP_ADR_GPP, FSP_PKT_WITHOUT_ACK,  &fsp_pkt);

    uint8_t  encoded_frame[FSP_PKT_MAX_LENGTH] = {0};
    uint8_t  frame_len;
    frame_encode(&fsp_pkt,  encoded_frame,  &frame_len);

    UART_FSP(&GPP_UART, encoded_frame, frame_len);

    return CMDLINE_OK;
}

int CMD_PULSE_DELAY(int argc, char *argv[])
{
    if (argc < 2)
        return CMDLINE_TOO_FEW_ARGS;
    else if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;

    int receive_argm;

    receive_argm = atoi(argv[1]);

    if ((receive_argm > 127) || (receive_argm < 2))
        return CMDLINE_INVALID_ARG;

    uint8_t  cmd  = FSP_CMD_PULSE_DELAY;
    uint8_t  payload = receive_argm;
    fsp_packet_t  fsp_pkt;
    fsp_gen_cmd_w_data_pkt(cmd,  &payload,  1, FSP_ADR_GPP, FSP_PKT_WITHOUT_ACK,  &fsp_pkt);

    uint8_t  encoded_frame[FSP_PKT_MAX_LENGTH] = {0};
    uint8_t  frame_len;
    frame_encode(&fsp_pkt,  encoded_frame,  &frame_len);

    UART_FSP(&GPP_UART, encoded_frame, frame_len);

    return CMDLINE_OK;
}

int CMD_PULSE_HV(int argc, char *argv[])
{
    if (argc < 3)
        return CMDLINE_TOO_FEW_ARGS;
    else if (argc > 3)
        return CMDLINE_TOO_MANY_ARGS;

    int receive_argm[2];

    receive_argm[0] = atoi(argv[1]);
    receive_argm[1] = atoi(argv[2]);

    if ((receive_argm[0] > 20) || (receive_argm[0] < 1))
        return CMDLINE_INVALID_ARG;
    else if ((receive_argm[1] > 20) || (receive_argm[1] < 1))
        return CMDLINE_INVALID_ARG;

    uint8_t  cmd  = FSP_CMD_PULSE_HV;
    uint8_t  payload[2];
    payload[0]  =  receive_argm[0];
    payload[1]  =  receive_argm[1];
    fsp_packet_t  fsp_pkt;
    fsp_gen_cmd_w_data_pkt(cmd,  payload,  sizeof(payload), FSP_ADR_GPP, FSP_PKT_WITHOUT_ACK,  &fsp_pkt);

    uint8_t  encoded_frame[FSP_PKT_MAX_LENGTH] = {0};
    uint8_t  frame_len;
    frame_encode(&fsp_pkt,  encoded_frame,  &frame_len);

    UART_FSP(&GPP_UART, encoded_frame, frame_len);

    return CMDLINE_OK;
}

int CMD_PULSE_LV(int argc, char *argv[])
{
    if (argc < 3)
        return CMDLINE_TOO_FEW_ARGS;
    else if (argc > 3)
        return CMDLINE_TOO_MANY_ARGS;
    
    int receive_argm[2];

    receive_argm[0] = atoi(argv[1]);
    receive_argm[1] = atoi(argv[2]);

    if ((receive_argm[0] > 500) || (receive_argm[0] < 1))
        return CMDLINE_INVALID_ARG;
    else if ((receive_argm[1] > 500) || (receive_argm[1] < 1))
        return CMDLINE_INVALID_ARG;

    uint8_t  cmd  = FSP_CMD_PULSE_LV;
    uint8_t  payload[2];
    payload[0]  =  receive_argm[0];
    payload[1]  =  receive_argm[1];
    fsp_packet_t  fsp_pkt;
    fsp_gen_cmd_w_data_pkt(cmd,  payload,  sizeof(payload), FSP_ADR_GPP, FSP_PKT_WITHOUT_ACK,  &fsp_pkt);

    uint8_t  encoded_frame[FSP_PKT_MAX_LENGTH] = {0};
    uint8_t  frame_len;
    frame_encode(&fsp_pkt,  encoded_frame,  &frame_len);

    UART_FSP(&GPP_UART, encoded_frame, frame_len);

    return CMDLINE_OK;
}

int CMD_PULSE_CONTROL(int argc, char *argv[])
{
    if (argc < 2)
        return CMDLINE_TOO_FEW_ARGS;
    else if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;

    int8_t receive_argm = atoi(argv[1]);

    if ((receive_argm > 1) || (receive_argm < 0))
        return CMDLINE_INVALID_ARG;

    uint8_t  cmd  = FSP_CMD_PULSE_CONTROL;
    uint8_t  payload = receive_argm;
    fsp_packet_t  fsp_pkt;
    fsp_gen_cmd_w_data_pkt(cmd,  &payload,  1, FSP_ADR_GPP, FSP_PKT_WITHOUT_ACK,  &fsp_pkt);

    uint8_t  encoded_frame[FSP_PKT_MAX_LENGTH] = {0};
    uint8_t  frame_len;
    frame_encode(&fsp_pkt,  encoded_frame,  &frame_len);

    UART_Write(&GPP_UART, &encoded_frame, frame_len);

    return CMDLINE_OK;
}

int CMD_RELAY_SET(int argc, char *argv[])
{
    if (argc < 3)
        return CMDLINE_TOO_FEW_ARGS;
    else if (argc > 3)
        return CMDLINE_TOO_MANY_ARGS;
    
    int receive_argm[2];

    receive_argm[0] = atoi(argv[1]);
    receive_argm[1] = atoi(argv[2]);

    if (receive_argm[0] == receive_argm[1])
        return CMDLINE_INVALID_ARG;
    else if ((receive_argm[0] > 7) || (receive_argm[0] < 0))
        return CMDLINE_INVALID_ARG;
    else if ((receive_argm[1] > 7) || (receive_argm[1] < 0))
        return CMDLINE_INVALID_ARG;

    uint8_t  cmd  = FSP_CMD_RELAY_SET;
    uint8_t  payload[2];
    payload[0]  =  receive_argm[0];
    payload[1]  =  receive_argm[1];
    fsp_packet_t  fsp_pkt;
    fsp_gen_cmd_w_data_pkt(cmd,  payload,  sizeof(payload), FSP_ADR_GPP, FSP_PKT_WITHOUT_ACK,  &fsp_pkt);

    uint8_t  encoded_frame[FSP_PKT_MAX_LENGTH] = {0};
    uint8_t  frame_len;
    frame_encode(&fsp_pkt,  encoded_frame,  &frame_len);

    UART_FSP(&GPP_UART, encoded_frame, frame_len);

    return CMDLINE_OK;
}

int CMD_RELAY_CONTROL(int argc, char *argv[])
{
    if (argc < 2)
        return CMDLINE_TOO_FEW_ARGS;
    else if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;

    int receive_argm;

    receive_argm = atoi(argv[1]);

    if ((receive_argm > 1) || (receive_argm < 0))
        return CMDLINE_INVALID_ARG;

    uint8_t  cmd  = FSP_CMD_RELAY_CONTROL;
    uint8_t  payload = receive_argm;
    fsp_packet_t  fsp_pkt;
    fsp_gen_cmd_w_data_pkt(cmd,  &payload,  1, FSP_ADR_GPP, FSP_PKT_WITHOUT_ACK,  &fsp_pkt);

    uint8_t  encoded_frame[FSP_PKT_MAX_LENGTH] = {0};
    uint8_t  frame_len;
    frame_encode(&fsp_pkt,  encoded_frame,  &frame_len);

    UART_FSP(&GPP_UART, encoded_frame, frame_len);

    return CMDLINE_OK;
}

int CMD_CHANNEL_SET(int argc, char *argv[])
{
    if (argc < 2)
        return CMDLINE_TOO_FEW_ARGS;
    else if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;

    int receive_argm;

    receive_argm = atoi(argv[1]);

    if ((receive_argm > 2) || (receive_argm < 1))
        return CMDLINE_INVALID_ARG;

    uint8_t  cmd  = FSP_CMD_CHANNEL_SET;
    uint8_t  payload = receive_argm;
    fsp_packet_t  fsp_pkt;
    fsp_gen_cmd_w_data_pkt(cmd,  &payload,  1, FSP_ADR_GPP, FSP_PKT_WITHOUT_ACK,  &fsp_pkt);

    uint8_t  encoded_frame[FSP_PKT_MAX_LENGTH] = {0};
    uint8_t  frame_len;
    frame_encode(&fsp_pkt,  encoded_frame,  &frame_len);

    UART_FSP(&GPP_UART, encoded_frame, frame_len);

    return CMDLINE_OK;
}

int CMD_CHANNEL_CONTROL(int argc, char *argv[])
{
    if (argc < 2)
        return CMDLINE_TOO_FEW_ARGS;
    else if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;

    int receive_argm;

    receive_argm = atoi(argv[1]);

    if ((receive_argm > 1) || (receive_argm < 0))
        return CMDLINE_INVALID_ARG;

    uint8_t  cmd  = FSP_CMD_CHANNEL_CONTROL;
    uint8_t  payload = receive_argm;
    fsp_packet_t  fsp_pkt;
    fsp_gen_cmd_w_data_pkt(cmd,  &payload,  1, FSP_ADR_GPP, FSP_PKT_WITHOUT_ACK,  &fsp_pkt);

    uint8_t  encoded_frame[FSP_PKT_MAX_LENGTH] = {0};
    uint8_t  frame_len;
    frame_encode(&fsp_pkt,  encoded_frame,  &frame_len);

    UART_FSP(&GPP_UART, encoded_frame, frame_len);

    return CMDLINE_OK;
}