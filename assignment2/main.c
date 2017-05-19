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
#include "s4396122_hal_tcp.h"
#include "s4396122_os_draw.h"

// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FreeRTOS_CLI.h"
#include "usbd_hid.h"
#include "usbd_conf.h"
#include "usbd_core.h"
#include "usbd_desc.h"

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
struct TaskHolder tasks[20];
int tasksPos;

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
    "char:\n Draw an alphanumeric character. (x, y, c)\n\n",
    prvCharCommand,
    3
};
CLI_Command_Definition_t xColour = {
    "colour",
    "colour:\n Change the colour of the pen\n\n",
    prvColourCommand,
    1
};
CLI_Command_Definition_t xText = {
    "text",
    "text:\n Draw a text string, up to 10 characters long. (x, y, str)\n\n",
    prvTextCommand,
    3
};
CLI_Command_Definition_t xPoly = {
    "poly",
    "poly:\n Draw a regular polygon. (x, y, l, o)\n\n",
    prvPolyCommand,
    4
};
CLI_Command_Definition_t xClear = {
    "clear",
    "clear:\n Clear the last drawn command\n\n",
    prvClearCommand,
    0
};

#define USBD_VID                   0x0483
#define USBD_PID                   0x572B
#define USBD_LANGID_STRING         0x409
#define USBD_MANUFACTURER_STRING   "STMicroelectronics"
#define USBD_PRODUCT_HS_STRING        "HID Joystick in HS Mode"
#define USBD_SERIALNUMBER_HS_STRING   "00000000001A"
#define USBD_PRODUCT_FS_STRING        "HID Joystick in FS Mode"
#define USBD_SERIALNUMBER_FS_STRING   "00000000001B"
#define USBD_CONFIGURATION_HS_STRING  "HID Config"
#define USBD_INTERFACE_HS_STRING      "HID Interface"
#define USBD_CONFIGURATION_FS_STRING  "HID Config"
#define USBD_INTERFACE_FS_STRING      "HID Interface"

uint8_t *     USBD_HID_DeviceDescriptor( USBD_SpeedTypeDef speed , uint16_t *length);
uint8_t *     USBD_HID_LangIDStrDescriptor( USBD_SpeedTypeDef speed , uint16_t *length);
uint8_t *     USBD_HID_ManufacturerStrDescriptor ( USBD_SpeedTypeDef speed , uint16_t *length);
uint8_t *     USBD_HID_ProductStrDescriptor ( USBD_SpeedTypeDef speed , uint16_t *length);
uint8_t *     USBD_HID_SerialStrDescriptor( USBD_SpeedTypeDef speed , uint16_t *length);
uint8_t *     USBD_HID_ConfigStrDescriptor( USBD_SpeedTypeDef speed , uint16_t *length);
uint8_t *     USBD_HID_InterfaceStrDescriptor( USBD_SpeedTypeDef speed , uint16_t *length);

#ifdef USB_SUPPORT_USER_STRING_DESC
uint8_t *     USBD_HID_USRStringDesc (USBD_SpeedTypeDef speed, uint8_t idx , uint16_t *length);  
#endif /* USB_SUPPORT_USER_STRING_DESC */  


USBD_DescriptorsTypeDef HID_Desc =
{
  USBD_HID_DeviceDescriptor,
  USBD_HID_LangIDStrDescriptor, 
  USBD_HID_ManufacturerStrDescriptor,
  USBD_HID_ProductStrDescriptor,
  USBD_HID_SerialStrDescriptor,
  USBD_HID_ConfigStrDescriptor,
  USBD_HID_InterfaceStrDescriptor,
  
};

/* USB Standard Device Descriptor */
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN uint8_t hUSBDDeviceDesc[USB_LEN_DEV_DESC] __ALIGN_END;

/* USB Standard Device Descriptor */
#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN uint8_t USBD_LangIDDesc[USB_LEN_LANGID_STR_DESC] __ALIGN_END;

#if defined ( __ICCARM__ ) /*!< IAR Compiler */
  #pragma data_alignment=4   
#endif
__ALIGN_BEGIN uint8_t USBD_StrDesc[USBD_MAX_STR_DESC_SIZ] __ALIGN_END;
/**
  * @}
  */ 


/** @defgroup USBD_DESC_Private_FunctionPrototypes
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup USBD_DESC_Private_Functions
  * @{
  */ 

/**
* @brief  USBD_HID_DeviceDescriptor 
*         return the device descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_HID_DeviceDescriptor( USBD_SpeedTypeDef speed , uint16_t *length)
{
  *length = sizeof(hUSBDDeviceDesc);
  return hUSBDDeviceDesc;
}

/**
* @brief  USBD_HID_LangIDStrDescriptor 
*         return the LangID string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_HID_LangIDStrDescriptor( USBD_SpeedTypeDef speed , uint16_t *length)
{
  *length =  sizeof(USBD_LangIDDesc);  
  return USBD_LangIDDesc;
}


/**
* @brief  USBD_HID_ProductStrDescriptor 
*         return the product string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_HID_ProductStrDescriptor( USBD_SpeedTypeDef speed , uint16_t *length)
{
  if(speed == 0)
  {   
    USBD_GetString ((uint8_t *)USBD_PRODUCT_HS_STRING, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString ((uint8_t *)USBD_PRODUCT_FS_STRING, USBD_StrDesc, length);
  }
  return USBD_StrDesc;
}

/**
* @brief  USBD_HID_ManufacturerStrDescriptor 
*         return the manufacturer string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_HID_ManufacturerStrDescriptor( USBD_SpeedTypeDef speed , uint16_t *length)
{
  USBD_GetString ((uint8_t *)USBD_MANUFACTURER_STRING, USBD_StrDesc, length);
  return USBD_StrDesc;
}

/**
* @brief  USBD_HID_SerialStrDescriptor 
*         return the serial number string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_HID_SerialStrDescriptor( USBD_SpeedTypeDef speed , uint16_t *length)
{
  if(speed  == USBD_SPEED_HIGH)
  {    
    USBD_GetString ((uint8_t *)USBD_SERIALNUMBER_HS_STRING, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString ((uint8_t *)USBD_SERIALNUMBER_FS_STRING, USBD_StrDesc, length);
  }
  return USBD_StrDesc;
}

/**
* @brief  USBD_HID_ConfigStrDescriptor 
*         return the configuration string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_HID_ConfigStrDescriptor( USBD_SpeedTypeDef speed , uint16_t *length)
{
  if(speed  == USBD_SPEED_HIGH)
  {  
    USBD_GetString ((uint8_t *)USBD_CONFIGURATION_HS_STRING, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString ((uint8_t *)USBD_CONFIGURATION_FS_STRING, USBD_StrDesc, length);
  }
  return USBD_StrDesc;  
}


/**
* @brief  USBD_HID_InterfaceStrDescriptor 
*         return the interface string descriptor
* @param  speed : current device speed
* @param  length : pointer to data length variable
* @retval pointer to descriptor buffer
*/
uint8_t *  USBD_HID_InterfaceStrDescriptor( USBD_SpeedTypeDef speed , uint16_t *length)
{
  if(speed == 0)
  {
    USBD_GetString ((uint8_t *)USBD_INTERFACE_HS_STRING, USBD_StrDesc, length);
  }
  else
  {
    USBD_GetString ((uint8_t *)USBD_INTERFACE_FS_STRING, USBD_StrDesc, length);
  }
  return USBD_StrDesc;  
}

USBD_HandleTypeDef device;

/**
 * Initializes the hardware
 */
void Hardware_init() {
    portDISABLE_INTERRUPTS();

    BRD_init();

    /*USBD_HID_Init(&device, 0);*/
    /*USBD_HID.Init(&device, 0);*/
    USBD_Init(&device, &HID_Desc, 0);
    USBD_RegisterClass(&device, &USBD_HID);
    USBD_Start(&device);
    
    s4396122_os_draw_init();

    BRD_LEDInit();
    BRD_LEDOn();

    tasksPos = 0;

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

    int8_t buff[4];
    buff[0] = 0x00; // Buttons
    buff[1] = 10; // X
    buff[2] = 10; // Y
    buff[3] = 255;

    /*int8_t buff2[5] = {0x02, 0, 0, 0, 0};*/


    while (1) {
        BRD_LEDToggle();
        USBD_HID_SendReport(&device, buff, 4);
        /*USBD_HID_SendReport(&device, buff2, 5);*/
        /*buff[3] *= -1;*/
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

    struct TaskHolder LEDTask = {"LED", &LED_Task, mainLED_TASK_STACK_SIZE, mainLED_PRIORITY};
    struct TaskHolder MainTask = {"Main", &main_Task, mainTask_TASK_STACK_SIZE, mainTask_PRIORITY};
    
    tasks[tasksPos++] = LEDTask;
    tasks[tasksPos++] = MainTask;

    for (int i = 0; i < tasksPos; i++) {
        xTaskCreate(tasks[i].function, tasks[i].name, tasks[i].stackDepth, NULL, tasks[i].priority, NULL);
    }
    
    // Create the LED Task
    /*xTaskCreate(&LED_Task, "LED", mainLED_TASK_STACK_SIZE, NULL, mainLED_PRIORITY, NULL);*/

    // Create Task to handle the serial/network input
    /*xTaskCreate(&main_Task, "Main", mainTask_TASK_STACK_SIZE, NULL, mainTask_PRIORITY, NULL);*/

    // Register CLI commands
    FreeRTOS_CLIRegisterCommand(&xTime);
    FreeRTOS_CLIRegisterCommand(&xTop);
    FreeRTOS_CLIRegisterCommand(&xSuspend);
    FreeRTOS_CLIRegisterCommand(&xResume);
    FreeRTOS_CLIRegisterCommand(&xDelete);
    FreeRTOS_CLIRegisterCommand(&xCreate);
    FreeRTOS_CLIRegisterCommand(&xChar);
    FreeRTOS_CLIRegisterCommand(&xColour);
    FreeRTOS_CLIRegisterCommand(&xText);
    FreeRTOS_CLIRegisterCommand(&xPoly);
    FreeRTOS_CLIRegisterCommand(&xClear);

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

    vTaskDelete(taskHandle);

    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}

static BaseType_t prvCreateCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {

    long paramLen;
    const char *taskName = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramLen);

    for (int i = 0; i < tasksPos; i++) {
        if (strcmp(taskName, tasks[i].name) == 0) {
            xTaskCreate(tasks[i].function, tasks[i].name, tasks[i].stackDepth, NULL, tasks[i].priority, NULL);
            xWriteBufferLen = sprintf(pcWriteBuffer, "");
            return pdFALSE;
        }
    }

    xWriteBufferLen = sprintf(pcWriteBuffer, "Could not create task\n");
    return pdFALSE;
}

static BaseType_t prvCharCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {

    long paramLen;
    const char *arguments = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramLen);

    int x, y;
    char c;
    sscanf(arguments, "%d %d %c", &x, &y, &c);
    s4396122_os_draw_add_char(x, y, c);

    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}

static BaseType_t prvColourCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {

    long paramLen;
    const char *colour = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramLen);
    s4396122_os_draw_change_pen(colour[0]);

    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}

static BaseType_t prvTextCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {

    long paramLen;
    const char *arguments = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramLen);

    int x, y;
    char str[12];
    sscanf(arguments, "%d %d %s", &x, &y, str);
    for (int i = 0; i < strlen(str); i++) {
        s4396122_os_draw_add_char(x + i, y, str[i]);
    }

    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}

static BaseType_t prvPolyCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {

    long paramLen;
    const char *arguments = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramLen);

    int x, y, length, degree;
    sscanf(arguments, "%d %d %d %d", &x, &y, &length, &degree);
    s4396122_os_draw_add_poly(x, y, length, degree);

    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}

static BaseType_t prvClearCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {

    s4396122_os_draw_remove_top();

    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}
