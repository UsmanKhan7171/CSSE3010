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

#define print_PRIORITY (tskIDLE_PRIORITY + 1)
#define print_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

struct DisplayPair {
    char *name;
    int *val;
};

extern void s4396122_os_print_init();
extern void s4396122_os_print_add_detail(char *name, int *val);
extern void s4396122_os_print(char *format, ...);

#endif
