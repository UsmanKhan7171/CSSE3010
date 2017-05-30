/**
 * @file s4396122_os_print.h
 * @brief Library to make console printing easier and less direct task 
 * intensive
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef OS_PRINT_H
#define OS_PRINT_H

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "s4396122_util_print.h"
#include "string.h"
#include "s4396122_util_queue.h"
#include <stdarg.h>
#include <debug_printf.h>

//! Print Task Priority
#define print_PRIORITY (tskIDLE_PRIORITY + 6)
//! Print Task Stack Size
#define print_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

/**
 * @brief Used to display a pair of string and integer auto updating pairs at
 * the top of the screen
 */
struct DisplayPair {
    char *name; //!< Name of the pair
    int *val;   //!< Auto updating integer
};

extern void s4396122_os_print_init();
extern void s4396122_os_print_add_detail(char *name, int *val);
extern void s4396122_os_print(char *format, ...);

#endif
