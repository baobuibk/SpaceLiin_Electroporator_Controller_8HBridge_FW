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
    { "MARCO",                  CMD_line_test,          "TEST" },
    { "GPC_CAP_VOLT",           GPC_CAP_VOLT,           "Set cap voltage"},
    { "GPC_CAP_CONTROL",        GPC_CAP_CONTROL,        "Control charger on/off"},
    { "GPC_CAP_RELEASE",        GPC_CAP_RELEASE,        "Control releasing cap"},
    { "GPC_PULSE_COUNT",        GPC_PULSE_COUNT,        "Set number of pulse" },
    { "GPC_PULSE_HS_DURATION",  GPC_PULSE_HS_DURATION,  "Set hs pulse on time and off time" },
    { "GPC_PULSE_LS_DURATION",  GPC_PULSE_LS_DURATION,  "Set ls pulse on time and off time" },
    { "GPC_PULSE_CONTROL",      GPC_PULSE_CONTROL,      "Start pulsing" },
    { "GPC_CUVETTE_ELECTRODE",  GPC_CUVETTE_ELECTRODE,  "Set up cuvette"},
    { "GPC_CUVETTE_CONTROL",    GPC_CUVETTE_CONTROL,    "Stop cuvette"},
	{0,0,0}
};
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Public Function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
int CMD_line_test(int argc, char *argv[])
{
    UART_Send_String(&RS232_UART, "> POLO\n");
    return CMDLINE_OK;
}

int GPC_CAP_VOLT(int argc, char *argv[])
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

    PID_300V_set_voltage    = (receive_argm[0] * 10) - 453;
    PID_50V_set_voltage     = (receive_argm[1] * 60) - 219;

    return CMDLINE_OK;
}

int GPC_CAP_CONTROL(int argc, char *argv[])
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

    PID_is_300V_on  = receive_argm[0];
    PID_is_50V_on   = receive_argm[1];

    return CMDLINE_OK;
}

int GPC_CAP_RELEASE(int argc, char *argv[])
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

    g_is_Discharge_300V_On  = receive_argm[0];
    g_is_Discharge_50V_On   = receive_argm[1];

    return CMDLINE_OK;
}

int GPC_PULSE_COUNT(int argc, char *argv[])
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

    uint8_t  cmd  = FSP_CMD_GPC_PULSE_COUNT;
    uint8_t  payload[2];
    payload[0]  =  receive_argm[0];
    payload[1]  =  receive_argm[1];
    fsp_packet_t  fsp_pkt;
    fsp_gen_cmd_w_data_pkt(cmd,  payload,  sizeof(payload), FSP_ADR_GPP, FSP_PKT_WITHOUT_ACK,  &fsp_pkt);

    uint8_t  encoded_frame[FSP_PKT_MAX_LENGTH] = {0};
    uint8_t  frame_len;
    frame_encode(&fsp_pkt,  encoded_frame,  &frame_len);

    UART_FSP(&GPP_UART, encoded_frame);

    return CMDLINE_OK;
}

int GPC_PULSE_HS_DURATION(int argc, char *argv[])
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

    uint8_t  cmd  = FSP_CMD_GPC_PULSE_HS_DURATION;
    uint8_t  payload[2];
    payload[0]  =  receive_argm[0];
    payload[1]  =  receive_argm[1];
    fsp_packet_t  fsp_pkt;
    fsp_gen_cmd_w_data_pkt(cmd,  payload,  sizeof(payload), FSP_ADR_GPP, FSP_PKT_WITHOUT_ACK,  &fsp_pkt);

    uint8_t  encoded_frame[FSP_PKT_MAX_LENGTH] = {0};
    uint8_t  frame_len;
    frame_encode(&fsp_pkt,  encoded_frame,  &frame_len);

    UART_FSP(&GPP_UART, encoded_frame);

    return CMDLINE_OK;
}

int GPC_PULSE_LS_DURATION(int argc, char *argv[])
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

    uint8_t  cmd  = FSP_CMD_GPC_PULSE_LS_DURATION;
    uint8_t  payload[2];
    payload[0]  =  receive_argm[0];
    payload[1]  =  receive_argm[1];
    fsp_packet_t  fsp_pkt;
    fsp_gen_cmd_w_data_pkt(cmd,  payload,  sizeof(payload), FSP_ADR_GPP, FSP_PKT_WITHOUT_ACK,  &fsp_pkt);

    uint8_t  encoded_frame[FSP_PKT_MAX_LENGTH] = {0};
    uint8_t  frame_len;
    frame_encode(&fsp_pkt,  encoded_frame,  &frame_len);

    UART_FSP(&GPP_UART, encoded_frame);

    return CMDLINE_OK;
}

int GPC_PULSE_CONTROL(int argc, char *argv[])
{
    if (argc < 2)
        return CMDLINE_TOO_FEW_ARGS;
    else if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;

    int receive_argm;

    receive_argm = atoi(argv[1]);

    if ((receive_argm > 1) || (receive_argm < 0))
        return CMDLINE_INVALID_ARG;

    uint8_t  cmd  = FSP_CMD_GPC_PULSE_CONTROL;
    uint8_t  payload = receive_argm;
    fsp_packet_t  fsp_pkt;
    fsp_gen_cmd_w_data_pkt(cmd,  payload,  sizeof(payload), FSP_ADR_GPP, FSP_PKT_WITHOUT_ACK,  &fsp_pkt);

    uint8_t  encoded_frame[FSP_PKT_MAX_LENGTH] = {0};
    uint8_t  frame_len;
    frame_encode(&fsp_pkt,  encoded_frame,  &frame_len);

    UART_FSP(&GPP_UART, encoded_frame);

    return CMDLINE_OK;
}

int GPC_CUVETTE_ELECTRODE(int argc, char *argv[])
{
    if (argc < 3)
        return CMDLINE_TOO_FEW_ARGS;
    else if (argc > 3)
        return CMDLINE_TOO_MANY_ARGS;
    
    int receive_argm[2];

    receive_argm[0] = atoi(argv[1]);
    receive_argm[1] = atoi(argv[2]);

    if ((receive_argm[0] > 7) || (receive_argm[0] < 0))
        return CMDLINE_INVALID_ARG;
    else if ((receive_argm[1] > 7) || (receive_argm[1] < 0))
        return CMDLINE_INVALID_ARG;

    uint8_t  cmd  = FSP_CMD_GPC_CUVETTE_ELECTRODE;
    uint8_t  payload[2];
    payload[0]  =  receive_argm[0];
    payload[1]  =  receive_argm[1];
    fsp_packet_t  fsp_pkt;
    fsp_gen_cmd_w_data_pkt(cmd,  payload,  sizeof(payload), FSP_ADR_GPP, FSP_PKT_WITHOUT_ACK,  &fsp_pkt);

    uint8_t  encoded_frame[FSP_PKT_MAX_LENGTH] = {0};
    uint8_t  frame_len;
    frame_encode(&fsp_pkt,  encoded_frame,  &frame_len);

    UART_FSP(&GPP_UART, encoded_frame);

    return CMDLINE_OK;
}

int GPC_CUVETTE_CONTROL(int argc, char *argv[])
{
    if (argc < 2)
        return CMDLINE_TOO_FEW_ARGS;
    else if (argc > 2)
        return CMDLINE_TOO_MANY_ARGS;

    int receive_argm;

    receive_argm = atoi(argv[1]);

    if ((receive_argm > 1) || (receive_argm < 0))
        return CMDLINE_INVALID_ARG;

    uint8_t  cmd  = FSP_CMD_GPC_CUVETTE_CONTROL;
    uint8_t  payload = receive_argm;
    fsp_packet_t  fsp_pkt;
    fsp_gen_cmd_w_data_pkt(cmd,  payload,  sizeof(payload), FSP_ADR_GPP, FSP_PKT_WITHOUT_ACK,  &fsp_pkt);

    uint8_t  encoded_frame[FSP_PKT_MAX_LENGTH] = {0};
    uint8_t  frame_len;
    frame_encode(&fsp_pkt,  encoded_frame,  &frame_len);

    UART_FSP(&GPP_UART, encoded_frame);

    return CMDLINE_OK;
}