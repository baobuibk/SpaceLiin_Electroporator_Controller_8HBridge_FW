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
int CMD_SET_CAP_VOLT(int argc, char *argv[]);
int CMD_SET_CAP_CONTROL(int argc, char *argv[]);
int CMD_SET_CAP_RELEASE(int argc, char *argv[]);

int CMD_GET_CAP_VOLT(int argc, char *argv[]);
int CMD_GET_CAP_CONTROL(int argc, char *argv[]);
int CMD_GET_CAP_RELEASE(int argc, char *argv[]);
int CMD_GET_CAP_ALL(int argc, char *argv[]);

/* :::::::::: Pulse Control Command :::::::: */
int CMD_SET_PULSE_POLE(int argc, char *argv[]);
int CMD_SET_PULSE_COUNT(int argc, char *argv[]);
int CMD_SET_PULSE_DELAY(int argc, char *argv[]);
int CMD_SET_PULSE_HV(int argc, char *argv[]);
int CMD_SET_PULSE_LV(int argc, char *argv[]);
int CMD_SET_PULSE_CONTROL(int argc, char *argv[]);

int CMD_GET_PULSE_POLE(int argc, char *argv[]);
int CMD_GET_PULSE_COUNT(int argc, char *argv[]);
int CMD_GET_PULSE_DELAY(int argc, char *argv[]);
int CMD_GET_PULSE_HV(int argc, char *argv[]);
int CMD_GET_PULSE_LV(int argc, char *argv[]);
int CMD_GET_PULSE_CONTROL(int argc, char *argv[]);
int CMD_GET_PULSE_ALL(int argc, char *argv[]);

/* :::::::::: VOM Command :::::::: */
int CMD_MEASURE_VOLT(int argc, char *argv[]);
int CMD_MEASURE_CURRENT(int argc, char *argv[]);
int CMD_MEASURE_IMPEDANCE(int argc, char *argv[]);

/* :::::::::: I2C Sensor Command :::::::: */
int CMD_GET_SENSOR_GYRO(int argc, char *argv[]);
int CMD_GET_SENSOR_ACCEL(int argc, char *argv[]);
int CMD_GET_SENSOR_LSM6DSOX(int argc, char *argv[]);

int CMD_GET_SENSOR_TEMP(int argc, char *argv[]);
int CMD_GET_SENSOR_PRESSURE(int argc, char *argv[]);
int CMD_GET_SENSOR_BMP390(int argc, char *argv[]);

/* :::::::::: Ultility Command :::::::: */
int CMD_HELP(int argc, char *argv[]);
int CMD_CALIB_RUN(int argc, char *argv[]);
int CMD_CALIB_MEASURE(int argc, char *argv[]);
int CMD_CALL_GPP(int argc, char *argv[]);
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ End of the program ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#endif /* COMMAND_H_ */
