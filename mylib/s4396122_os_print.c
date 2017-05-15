#include "s4396122_os_print.h"

SemaphoreHandle_t s4396122_SemaphoreAddPrint;
Queue *buffer;
SemaphoreHandle_t s4396122_SemaphoreAddDetail;
Queue *details;
TaskHandle_t s4396122_TaskPrintSerialHandle;

void s4396122_TaskPrintSerial() {
    vTaskDelay(1000);
    debug_printf("\033[2J\033[1;1H");
    debug_printf("Printer\n");

    while (1) {
        debug_printf("\033[u");
        if (s4396122_SemaphoreAddPrint != NULL && xSemaphoreTake(s4396122_SemaphoreAddPrint, 1000)) {
            /*debug_printf("\033[1;25H");*/
            while (1) {
                char *str = (char *) s4396122_util_queue_pop(buffer);
                if (str == NULL)
                    break;

                debug_printf(str);
                free(str);
            }
            xSemaphoreGive(s4396122_SemaphoreAddPrint);
        }
        debug_printf("\033[s");

        if (s4396122_SemaphoreAddDetail != NULL && xSemaphoreTake(s4396122_SemaphoreAddDetail, 1000)) {
            Queue *tempDetails = s4396122_util_queue_create();
            debug_printf("\033[1;1H");
            while (1) {
                struct DisplayPair *dp = (struct DisplayPair *) s4396122_util_queue_pop(details);
                if (dp == NULL)
                    break;
                s4396122_util_queue_push(tempDetails, dp);
                debug_printf("%s: %d\n", dp->name, *(dp->val));
            }
        }

        vTaskDelay(250);
    }
}

void s4396122_os_print_init() {
    s4396122_SemaphoreAddPrint = xSemaphoreCreateMutex();
    s4396122_SemaphoreAddDetail = xSemaphoreCreateMutex();
    if (s4396122_SemaphoreAddPrint != NULL && xSemaphoreTake(s4396122_SemaphoreAddPrint, 1000)) {
        buffer = s4396122_util_queue_create();
        xSemaphoreGive(s4396122_SemaphoreAddPrint);
    }
    if (s4396122_SemaphoreAddDetail != NULL && xSemaphoreTake(s4396122_SemaphoreAddDetail, 1000)) {
        details = s4396122_util_queue_create();
        xSemaphoreGive(s4396122_SemaphoreAddDetail);
    }
    xTaskCreate(&s4396122_TaskPrintSerial, "Serial", print_TASK_STACK_SIZE, NULL, 
            print_PRIORITY, &s4396122_TaskPrintSerialHandle);
}

void s4396122_os_print(char *format, ...) {
    if (s4396122_SemaphoreAddPrint != NULL) {
        if (xSemaphoreTake(s4396122_SemaphoreAddPrint, portMAX_DELAY)) {
            va_list valist;
            char *tempString = malloc(sizeof(char) * 1024);
            memset(tempString, '\0', 1024);
            va_start(valist, format);
            vsprintf(tempString, format, valist);
            va_end(valist);
            s4396122_util_queue_push(buffer, tempString);
            xSemaphoreGive(s4396122_SemaphoreAddPrint);
        }
    }
}

void s4396122_os_print_add_detail(char *name, int *val) {
    if (s4396122_SemaphoreAddDetail != NULL) {
        if (xSemaphoreTake(s4396122_SemaphoreAddDetail, portMAX_DELAY)) {
            struct DisplayPair *dp = malloc(sizeof(struct DisplayPair));
            char *tempString = malloc(sizeof(char) * 1024);
            memset(tempString, '\0', 1024);
            dp->name = name;
            dp->val = val;
            s4396122_util_queue_push(details, dp);
        }
    }
}
