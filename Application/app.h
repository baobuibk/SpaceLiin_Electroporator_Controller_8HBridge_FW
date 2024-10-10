#ifndef APP_H_
#define APP_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "stm32f030xc.h"

// SYSTEM DRIVER //
#include "board.h"

// USER DRIVER //
#include "scheduler.h"

// INCLUDE TASK //
#include "adc_task.h"
#include "pid_task.h"
#include "discharge_task.h"
#include "cmd_line_task.h"
#include "calib_task.h"
#include "fsp_line_task.h"
#include "impedance_task.h"

#include "BMP390.h"
#include "command.h"

// INCLUDE LIB //
#include "uart.h"

void App_Main(void);

#endif /* APP_H_ */
