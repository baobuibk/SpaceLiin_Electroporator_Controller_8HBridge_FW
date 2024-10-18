#ifndef COMMAND_H_
#define COMMAND_H_

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Include ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include <stdint.h>
#include "app.h"
#include "cmd_line.h"
#include "fsp_line_task.h"
#include "fsp.h"
#include "crc.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Defines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Enum ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Struct ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Class ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Types ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Variables ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Prototype ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* :::::::::: Cap Control Command :::::::: */
int CMD_CAP_VOLT(int argc, char *argv[]);
int CMD_CAP_CONTROL(int argc, char *argv[]);
int CMD_CAP_RELEASE(int argc, char *argv[]);

/* :::::::::: Pulse Control Command :::::::: */
int CMD_PULSE_COUNT(int argc, char *argv[]);
int CMD_PULSE_DELAY(int argc, char *argv[]);
int CMD_PULSE_HV(int argc, char *argv[]);
int CMD_PULSE_LV(int argc, char *argv[]);
int CMD_PULSE_CONTROL(int argc, char *argv[]);

/* :::::::::: Relay Control Command :::::::: */
int CMD_RELAY_SET(int argc, char *argv[]);
int CMD_RELAY_CONTROL(int argc, char *argv[]);

/* :::::::::: VOM Command :::::::: */
int CMD_MEASURE_VOLT(int argc, char *argv[]);
int CMD_MEASURE_CURRENT(int argc, char *argv[]);
int CMD_MEASURE_IMPEDANCE(int argc, char *argv[]);

/* :::::::::: Ultility Command :::::::: */
int CMD_HELP(int argc, char *argv[]);
int CMD_CALIB_RUN(int argc, char *argv[]);
int CMD_CALIB_MEASURE(int argc, char *argv[]);
int CMD_CHANNEL_SET(int argc, char *argv[]);
int CMD_CHANNEL_CONTROL(int argc, char *argv[]);
int CMD_CALL_GPP(int argc, char *argv[]);
int CMD_GET_BMP390(int argc, char *argv[]);
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#endif /* COMMAND_H_ */
