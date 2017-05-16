/**
 * *****************************************************************************
 * @file    stage4a/main.c
 * @author  Daniel Fitzmaurice
 * @date    240417
 * @brief   Main file for the stage4a design task
 * *****************************************************************************
 */

// Includes
#include "FreeRTOSConfig.h"
#include "board.h"
#include <stdio.h>

// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "s4396122_hal_tcp.h"
#include "FreeRTOS_CLI.h"

// Task Priorities
#define mainLED_PRIORITY (tskIDLE_PRIORITY + 3)
#define mainLED_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define mainTask_PRIORITY (tskIDLE_PRIORITY + 2)
#define mainTask_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 6)

// Task Holding struct
struct TaskHolder {
    char *name;
    TaskFunction_t function;
    unsigned short stackDepth;
    UBaseType_t priority;
};

// Global variables
struct tcpConnection currentConn;
char *pcOutputString;
struct TaskHolder deleteTasks[20];
int deleteTasksPos;

// Commands
static BaseType_t prvTimeCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvTopCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvSuspendCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvResumeCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvDeleteCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvCreateCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvCharCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvColourCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvTextCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvPolyCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvClearCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

// Command Variables
CLI_Command_Definition_t xTime = {
    "time",
    "time:\n Display how long the program has been running for.\n\n",
    prvTimeCommand,
    0
};
CLI_Command_Definition_t xTop = {
    "top",
    "top:\n List the current number of tasks running.\n\n",
    prvTopCommand,
    0
};
CLI_Command_Definition_t xSuspend = {
    "suspend",
    "suspend:\n Suspend a mylib OS peripheral task with a given name\n\n",
    prvSuspendCommand,
    1
};
CLI_Command_Definition_t xResume = {
    "resume",
    "resume:\n Resume a mylib OS peripheral task with a given name\n\n",
    prvResumeCommand,
    1
};
CLI_Command_Definition_t xDelete = {
    "delete",
    "delete:\n Delete a mylib OS peripheral task with a given name\n\n",
    prvDeleteCommand,
    1
};
CLI_Command_Definition_t xCreate = {
    "create",
    "create:\n Create a mylib OS peripheral task with a given name\n\n",
    prvCreateCommand,
    1
};
CLI_Command_Definition_t xChar = {
    "char",
    "char:\n Draw an alphanumeric character\n\n",
    prvCharCommand,
    1
};
CLI_Command_Definition_t xColour = {
    "colour",
    "colour:\n Change the colour of the pen\n\n",
    prvColourCommand,
    1
};
CLI_Command_Definition_t xText = {
    "text",
    "text:\n Draw a text string, up to 10 characters long\n\n",
    prvTextCommand,
    1
};
CLI_Command_Definition_t xPoly = {
    "poly",
    "poly:\n Draw a regular polygon\n\n",
    prvPolyCommand,
    1
};
CLI_Command_Definition_t xClear = {
    "clear",
    "clear:\n Clear the last drawn command\n\n",
    prvClearCommand,
    1
};

/**
 * Initializes the hardware
 */
void Hardware_init() {
    portDISABLE_INTERRUPTS();

    BRD_init();

    BRD_LEDInit();
    BRD_LEDOn();

    deleteTasksPos = 0;

    portENABLE_INTERRUPTS();
}

/**
 * vApplicationStackOverflowHook
 * @param pxTask     Task Handler
 * @param pcTaskName Task Name
 */
void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName) {
    BRD_LEDOn();
    (void) pxTask;
    (void) pcTaskName;

    while (1);
}

void process_command_queue(IntQueue *q) {
    BaseType_t xReturned = pdTRUE;
    char cInputString[100];
    memset(cInputString, 0, sizeof(cInputString));
    s4396122_util_int_queue_to_string(q, cInputString);
    IntQueue *output = s4396122_util_int_queue_create();

    while (xReturned != pdFALSE) {
        xReturned = FreeRTOS_CLIProcessCommand(cInputString, pcOutputString, configCOMMAND_INT_MAX_OUTPUT_SIZE);

        s4396122_util_int_queue_from_string(output, pcOutputString);
        vTaskDelay(50);
    }
    s4396122_util_int_queue_from_string(output, ":\^) ");
    s4396122_hal_tcp_print(&currentConn, output);
    s4396122_util_int_queue_free(output);
}

/**
 * @brief Task for handling the network input and sending the input to the 
 * process functions
 */
void main_Task() {
    s4396122_hal_tcp_init();

    pcOutputString = FreeRTOS_CLIGetOutputBuffer();

    while (1) {
        currentConn = s4396122_hal_tcp_accept();

        char buffer[50];
        IntQueue *iq = s4396122_util_int_queue_create();
        s4396122_util_int_queue_from_string(iq, "Network Connected\n:\^) ");
        s4396122_hal_tcp_print(&currentConn, iq);
        s4396122_util_int_queue_free(iq);

        while (1) {
            s4396122_hal_tcp_read(&currentConn, &process_command_queue);
            if (!currentConn.open)
                break;
            vTaskDelay(100);
        }
    }
}

/**
 * Blinking LED Task to ensure a visual feedback that system is alive
 */
void LED_Task() {
    while (1) {
        BRD_LEDToggle();
        vTaskDelay(250);
    }
}

/**
 * Main system loop
 * @return Exit code status
 */
int main() {

    Hardware_init(); // Initialise peripherials
    LwIP_Init();
    
    // Create the LED Task
    xTaskCreate(&LED_Task, "LED", mainLED_TASK_STACK_SIZE, NULL,
        mainLED_PRIORITY, NULL);

    // Create Task to handle the serial/network input
    xTaskCreate(&main_Task, "Main", mainTask_TASK_STACK_SIZE, NULL, mainTask_PRIORITY, NULL);

    // Register CLI commands
    FreeRTOS_CLIRegisterCommand(&xTime);
    FreeRTOS_CLIRegisterCommand(&xTop);
    FreeRTOS_CLIRegisterCommand(&xSuspend);
    FreeRTOS_CLIRegisterCommand(&xResume);
    FreeRTOS_CLIRegisterCommand(&xDelete);
    FreeRTOS_CLIRegisterCommand(&xCreate);

    // Hands main loop control over to freertos. This should never exit
    vTaskStartScheduler();
}

static BaseType_t prvTimeCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    xWriteBufferLen = sprintf((char *) pcWriteBuffer, "Ticks: %d\n", HAL_GetTick());
    return pdFALSE;
}

static BaseType_t prvTopCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {

    unsigned long ulTotalRunTime;
    UBaseType_t uxArraySize = uxTaskGetNumberOfTasks();
    TaskStatus_t *pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));
    uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);
    ulTotalRunTime /= 100UL;

    char *pcWriteBufferPos = pcWriteBuffer;
    int bufferSize = 0;

    int writeSize = sprintf(pcWriteBufferPos, "   %-9s  %9s  %9s  %9s  %9s  %9s\n", "Name", "Priority", "Run Time", "CPU Usage", "Stack Rem", "Status");
    pcWriteBufferPos += writeSize;
    bufferSize += writeSize;

    for (UBaseType_t i = 0; i < uxArraySize; i++) {
        unsigned long ulStatsAsPercentage = pxTaskStatusArray[i].ulRunTimeCounter / ulTotalRunTime;
        char *stateName;
        switch(pxTaskStatusArray[i].eCurrentState) {
            case eReady:
                stateName = "Ready";
                break;
            case eRunning:
                stateName = "Running";
                break;
            case eBlocked:
                stateName = "Blocked";
                break;
            case eSuspended:
                stateName = "Suspended";
                break;
            case eDeleted:
                stateName = "Deleted";
                break;
            default:
                stateName = "Unknown";
                break;
        }
        int writeSize = sprintf(pcWriteBufferPos, "%2d:%-9s  %9lu  %9lu  %8lu%%  %9u  %9s\n",
                pxTaskStatusArray[i].xTaskNumber,
                pxTaskStatusArray[i].pcTaskName,
                pxTaskStatusArray[i].uxCurrentPriority,
                pxTaskStatusArray[i].ulRunTimeCounter,
                ulStatsAsPercentage,
                pxTaskStatusArray[i].usStackHighWaterMark,
                stateName
                );
        pcWriteBufferPos += writeSize;
        bufferSize += writeSize;
    }

    xWriteBufferLen = bufferSize;

    return pdFALSE;
}

static BaseType_t prvSuspendCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {

    long paramLen;
    const char *taskName = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramLen);

    TaskHandle_t taskHandle = xTaskGetHandle(taskName);
    if (taskHandle == NULL) {
        xWriteBufferLen = sprintf(pcWriteBuffer, "Invalid Task Name\n");
        return pdFALSE;
    }

    vTaskSuspend(taskHandle);

    xWriteBufferLen = sprintf(pcWriteBuffer, "");

    return pdFALSE;
}

static BaseType_t prvResumeCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {

    long paramLen;
    const char *taskName = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramLen);

    TaskHandle_t taskHandle = xTaskGetHandle(taskName);
    if (taskHandle == NULL) {
        xWriteBufferLen = sprintf(pcWriteBuffer, "Invalid Task Name\n");
        return pdFALSE;
    }

    vTaskResume(taskHandle);

    xWriteBufferLen = sprintf(pcWriteBuffer, "");

    return pdFALSE;
}

static BaseType_t prvDeleteCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    
    long paramLen;
    const char *taskName = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramLen);

    TaskHandle_t taskHandle = xTaskGetHandle(taskName);
    if (taskHandle == NULL) {
        xWriteBufferLen = sprintf(pcWriteBuffer, "Invalid Task Name\n");
        return pdFALSE;
    }
    TaskStatus_t taskStatus;
    vTaskGetInfo(taskHandle, &taskStatus, pdFALSE, eRunning);

    struct TaskHolder currentTask;
    currentTask.name = taskStatus.pcTaskName;
    currentTask.priority = taskStatus.uxCurrentPriority;
    currentTask.stackDepth = configMINIMAL_STACK_SIZE * 4;
    currentTask.function = &LED_Task;

    deleteTasks[deleteTasksPos++] = currentTask;
    if (deleteTasksPos >= 20)
        deleteTasksPos = 19;

    vTaskDelete(taskHandle);

    xWriteBufferLen = sprintf(pcWriteBuffer, "");

    return pdFALSE;
}

static BaseType_t prvCreateCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {

    long paramLen;
    const char *taskName = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramLen);

    for (int i = 0; i < deleteTasksPos; i++) {
        if (strcmp(taskName, deleteTasks[i].name) == 0) {
            xTaskCreate(deleteTasks[i].function, deleteTasks[i].name, deleteTasks[i].stackDepth, NULL, deleteTasks[i].priority, NULL);
            xWriteBufferLen = sprintf(pcWriteBuffer, "");
            return pdFALSE;
        }
    }

    xWriteBufferLen = sprintf(pcWriteBuffer, "Could not create task\n");
    return pdFALSE;
}

static BaseType_t prvCharCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}

static BaseType_t prvColourCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}

static BaseType_t prvTextCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}

static BaseType_t prvPolyCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}

static BaseType_t prvClearCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}
