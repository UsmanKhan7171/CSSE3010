/**
 * @file main.c
 * @brief Main file for project 2
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-30
 */

// Includes
#include "FreeRTOSConfig.h"
#include "board.h"
#include <stdio.h>
#include "s4396122_hal_tcp.h"
#include "s4396122_os_draw.h"
#include "s4396122_cli_draw.h"
#include "s4396122_hal_irremote.h"
#include "s4396122_hal_ir.h"
#include "s4396122_util_map.h"
#include "s4396122_util_int_queue.h"
#include "s4396122_hal_joystick.h"
#include "s4396122_os_joystick.h"
#include "s4396122_cli_joystick.h"
#include "s4396122_hal_accel.h"
#include "s4396122_os_mqtt.h"

// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "FreeRTOS_CLI.h"

/**
 * @brief Fix for the VCP_txflush function call in mqtt library
 */
void VCP_txflush() {
}

//! Main LED priority
#define mainLED_PRIORITY (tskIDLE_PRIORITY + 4)
//! Main LED stack size
#define mainLED_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
//! Main CLI task priority
#define mainTask_PRIORITY (tskIDLE_PRIORITY + 3)
//! Main CLI task stack size
#define mainTask_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 8)
//! Drawer task priority
#define drawerTask_PRIORITY (tskIDLE_PRIORITY + 3)
//! Drawer task stack size
#define drawerTask_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
//! IR task priority
#define IRTask_PRIORITY (tskIDLE_PRIORITY + 2)
//! IR task stack size
#define IRTask_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 5)
//! Joystick priority
#define joystickTask_PRIORITY (tskIDLE_PRIORITY + 2)
//! Joystick task stack size
#define joystickTask_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 5)
//! Accel priority
#define accelTask_PRIORITY (tskIDLE_PRIORITY + 2)
//! Accel task stack size
#define accelTask_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 5)

/**
 * @brief Struct for holder all the information required to re-create a task
 */
struct TaskHolder {
    char *name;                 //!< Name of the task
    TaskFunction_t function;    //!< The function associated with the task
    unsigned short stackDepth;  //!< The amount of stack allocated to the task
    UBaseType_t priority;       //!< The task priority
};

struct tcpConnection currentConn;   //!< Struct for current network connection
char *pcOutputString;               //!< Buffer for the output from cli command
struct TaskHolder tasks[20];        //!< Array of tasks that can be created
int tasksPos;                       //!< The current position in tasks array

//! The last received char from the IR remote
char lastChar = 'C';
//! The number of times the same char has occured from the IR remote
int numOccur = 0;

// All the CLI Commands required for the project 2 CLI design tasks
static BaseType_t prvTimeCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString);
static BaseType_t prvTopCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString);
static BaseType_t prvSuspendCommand(char *pcWriteBuffer, 
        size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvResumeCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
        const char *pcCommandString);
static BaseType_t prvDeleteCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
        const char *pcCommandString);
static BaseType_t prvCreateCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
        const char *pcCommandString);
static BaseType_t prvCharCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString);
static BaseType_t prvColourCommand(char *pcWriteBuffer, size_t xWriteBufferLen,
        const char *pcCommandString);
static BaseType_t prvTextCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString);
static BaseType_t prvPolyCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString);
static BaseType_t prvClearCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString);

/**
 * @brief CLI struct for time command
 */
CLI_Command_Definition_t xTime = {
    "time",
    "time:\n Display how long the program has been running for.\n\n",
    prvTimeCommand,
    0
};
/**
 * @brief CLI struct for top command
 */
CLI_Command_Definition_t xTop = {
    "top",
    "top:\n List the current number of tasks running.\n\n",
    prvTopCommand,
    0
};
/**
 * @brief CLI struct for suspend command
 */
CLI_Command_Definition_t xSuspend = {
    "suspend",
    "suspend:\n Suspend a mylib OS peripheral task with a given name\n\n",
    prvSuspendCommand,
    1
};
/**
 * @brief CLI struct for resume command
 */
CLI_Command_Definition_t xResume = {
    "resume",
    "resume:\n Resume a mylib OS peripheral task with a given name\n\n",
    prvResumeCommand,
    1
};
/**
 * @brief CLI struct for delete command
 */
CLI_Command_Definition_t xDelete = {
    "delete",
    "delete:\n Delete a mylib OS peripheral task with a given name\n\n",
    prvDeleteCommand,
    1
};
/**
 * @brief CLI struct for create command
 */
CLI_Command_Definition_t xCreate = {
    "create",
    "create:\n Create a mylib OS peripheral task with a given name\n\n",
    prvCreateCommand,
    1
};
/**
 * @brief CLI struct for char command
 */
CLI_Command_Definition_t xChar = {
    "char",
    "char:\n Draw an alphanumeric character. (x, y, c)\n\n",
    prvCharCommand,
    3
};
/**
 * @brief CLI struct for colour command
 */
CLI_Command_Definition_t xColour = {
    "colour",
    "colour:\n Change the colour of the pen\n\n",
    prvColourCommand,
    1
};
/**
 * @brief CLI struct for text command
 */
CLI_Command_Definition_t xText = {
    "text",
    "text:\n Draw a text string, up to 10 characters long. (x, y, str)\n\n",
    prvTextCommand,
    3
};
/**
 * @brief CLI struct for poly command
 */
CLI_Command_Definition_t xPoly = {
    "poly",
    "poly:\n Draw a regular polygon. (x, y, l, o)\n\n",
    prvPolyCommand,
    4
};
/**
 * @brief CLI struct for clear command
 */
CLI_Command_Definition_t xClear = {
    "clear",
    "clear:\n Clear the last drawn command\n\n",
    prvClearCommand,
    0
};

/**
 * @brief Initializes the systems hardware and calls the libraries init 
 * functions
 */
void Hardware_init() {

    portDISABLE_INTERRUPTS();

    s4396122_os_draw_init();
    s4396122_cli_draw_init();
    s4396122_hal_irremote_init();
    s4396122_hal_ir_init();
    s4396122_hal_joystick_init();
    s4396122_os_joystick_init();
    s4396122_cli_joystick_init();
    s4396122_hal_accel_init();

    BRD_LEDInit();
    BRD_LEDOn();

    tasksPos = 0;

    portENABLE_INTERRUPTS();
}

/**
 * @brief Called whenever the system stack overflows
 *
 * @param pxTask        Task Handler for the overflowed task
 * @param pcTaskName    Task Name of the overflowed task
 */
void vApplicationStackOverflowHook(xTaskHandle pxTask, 
        signed char *pcTaskName) {

    BRD_LEDOff();
    (void) pxTask;
    (void) pcTaskName;

    while (1);
}

/**
 * @brief Processes the input cli command and calls the required FreeRTOS 
 * Command
 *
 * @param q IntQueue of the input received from the CLI
 */
void process_command_queue(IntQueue *q) {

    // If pdFALSE then command needs to run again
    BaseType_t xReturned = pdTRUE;  
    // CLI command input buffer
    char cInputString[100];
    memset(cInputString, 0, sizeof(cInputString));
    // Convert the IntQueue to string for FreeRTOS
    s4396122_util_int_queue_to_string(q, cInputString);
    // Contains the output from the command
    IntQueue *output = s4396122_util_int_queue_create();

    while (xReturned != pdFALSE) {

        // Call the required FreeRTOS command
        xReturned = FreeRTOS_CLIProcessCommand(cInputString, pcOutputString, 
                configCOMMAND_INT_MAX_OUTPUT_SIZE);

        // Add the output from the command to the output buffer
        s4396122_util_int_queue_from_string(output, pcOutputString);
        vTaskDelay(50);
    }

    // Add the shell output string to the output to reduce network double
    s4396122_util_int_queue_from_string(output, ":\\^) ");
    // Print the result to the tcp connection
    s4396122_hal_tcp_print(&currentConn, output);
    s4396122_util_int_queue_free(output);
}

/**
 * @brief Called whenever there is a command sent over an mqtt connection
 *
 * @param buffer    CLI input that was received
 */
void mqtt_input(char *buffer) {

    // Create a queue to add the buffer to so it can easily be called with
    // process_command_queue
    IntQueue *output = s4396122_util_int_queue_create();
    s4396122_util_int_queue_from_string(output, buffer);
    process_command_queue(output);
    s4396122_util_int_queue_free(output);
}

/**
 * @brief Task for handling the network input and sending the input to the 
 * process functions
 */
void main_Task() {

    // Initialize the tcp library
    s4396122_hal_tcp_init();

    // Create a buffer for the CLI Commands to output to
    pcOutputString = FreeRTOS_CLIGetOutputBuffer();

    while (1) {

        // Wait for an active tcp connection
        currentConn = s4396122_hal_tcp_accept();

        // Create an IntQueue and print out the welcome message to the network
        // connection
        IntQueue *iq = s4396122_util_int_queue_create();
        s4396122_util_int_queue_from_string(iq, "Network Connected\n:\\^) ");
        s4396122_hal_tcp_print(&currentConn, iq);
        s4396122_util_int_queue_free(iq);

        while (1) {

            // While the connection is still active, read from the connection 
            // and handle any input
            s4396122_hal_tcp_read(&currentConn, &process_command_queue);
            // If the connection has closed for some reason, break out of the 
            // loop and accept a new connection
            if (!currentConn.open) {

                break;
            }

            vTaskDelay(100);
        }
    }
}

/**
 * @brief Handler for when a user presses a numeric button on the IR Remote
 *
 * @param c character representing the character pressed
 */
void num_enter(char c) {

    // If the character is the same as the last one entered, increment the
    // count, otherwise start again
    if (lastChar == c) {

        numOccur++;
    } else {

        lastChar = c;
        numOccur = 0;
    }

    char enteredChar;   //!< The resulting incoded char of the input
    // If the IR Remote hasn't had a relevant number input
    if (lastChar == 'C') {

        return;
    }

    // Switch for handling all number character inputs and setting the correct 
    // segment representation
    switch(lastChar) {

        case '0':

            enteredChar = s4396122_os_draw_number_to_segment[0];
            break;

        case '1':

            enteredChar = s4396122_os_draw_number_to_segment[1];
            break;

        case '2':

        case '3':

        case '4':

        case '5':

        case '6':

            if ((numOccur % 4) == 0) {

                enteredChar = s4396122_os_draw_number_to_segment[
                    lastChar - '0'];
            } else {

                enteredChar = s4396122_os_draw_letter_to_segment[
                    3 * (lastChar - '2') + (numOccur % 4) - 1];
            }
            break;

        case '7':

            if ((numOccur % 5) == 0) {

                enteredChar = s4396122_os_draw_number_to_segment[7];
            } else {

                enteredChar = s4396122_os_draw_letter_to_segment[
                    (numOccur % 5) - 1 + ('P' - 'A')];
            }
            break;

        case '8':

            if ((numOccur % 4) == 0) {

                enteredChar = s4396122_os_draw_number_to_segment[8];
            } else {

                enteredChar = s4396122_os_draw_letter_to_segment[
                    (numOccur % 4) - 1 + ('T' - 'A')];
            }
            break;

        case '9':

            if ((numOccur % 5) == 0) {

                enteredChar = s4396122_os_draw_number_to_segment[9];
            } else {

                enteredChar = s4396122_os_draw_letter_to_segment[
                    (numOccur % 5) - 1 + ('W' - 'A')];
            }
            break;

    }
    s4396122_os_draw_add_temp_char(enteredChar);
}

/**
 * @brief Handler for when the user presses a directional button on the IR 
 * remote
 *
 * @param c Character that was pressed on the remote
 */
void num_move(char c) {

    switch (c) {

        case '<':

            s4396122_os_draw_move_temp_char(-1, 0);
            break;

        case '>':

            s4396122_os_draw_move_temp_char(1, 0);
            break;

        case '+':

            s4396122_os_draw_move_temp_char(0, -1);
            break;

        case '-':

            s4396122_os_draw_move_temp_char(0, 1);
            break;

    }
}

/**
 * @brief Handler for when the user presses the back button
 *
 * @param c Character that was pressed on the remote
 */
void num_commit(char c) {

    s4396122_os_draw_commit_temp_char();
}

/**
 * @brief Handler for when the user presses the play button
 *
 * @param c Character that was pressed on the remote
 */
void num_clear(char c) {

    s4396122_os_draw_remove_top();
}

/**
 * @brief Task for handling the ir remote input and calling the required 
 * functions
 */
void IR_Task() {

    // A map for quickly looking up functions for the ir
    Map *remoteMap = s4396122_util_map_create();
    int irEnabled = 1;  // variable for controlling the ir enabled state

    // Add all the mappings for the ir remote and function calls
    for (int i = '0'; i <= '9'; i++) {
        s4396122_util_map_add(remoteMap, i, &num_enter);
    }
    s4396122_util_map_add(remoteMap, (int) '<', &num_move);
    s4396122_util_map_add(remoteMap, (int) '>', &num_move);
    s4396122_util_map_add(remoteMap, (int) '+', &num_move);
    s4396122_util_map_add(remoteMap, (int) '-', &num_move);
    s4396122_util_map_add(remoteMap, (int) 'B', &num_commit);
    s4396122_util_map_add(remoteMap, (int) 'P', &num_clear);

    while (1) {

        // Read from the ir queue and process the queue for the irremote
        s4396122_hal_irremote_process(s4396122_hal_ir_get_queue());

        if (s4396122_hal_irremote_input_available()) {

            char c = s4396122_hal_irremote_get_char();

            // Create a buffer and publish the ir character to the mqtt stream
            char buffer[10];
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "%c", c);
            s4396122_os_mqtt_publish("ir", buffer);

            if (c == '@') {

                // Toggle the ir state
                irEnabled ^= 1;
            }
            // Get the ir remote function from the map
            void (*f)(char q) = s4396122_util_map_get(remoteMap, (int) c);
            if (f == NULL) {

                // The mapping doesn't have a function
                continue;
            }
            // If the IR is enabled then call the function
            if (irEnabled) {

                f(c);
            }
        }
        vTaskDelay(200);
    }
}

/**
 * @brief Task for handling the joystick input and setting the origin
 */
void joystick_Task() {

    int x = 0;
    int y = 0;

    while (1) {

        // Read the joystick input and move the mouse coords accordingly
        x += (s4396122_hal_joystick_x_read() - 2048)/100;
        y += (s4396122_hal_joystick_y_read() - 2048)/100;

        // If the joystick isn't at the origin position
        if (x != 0 && y != 0) {

            s4396122_os_draw_move_mouse(-x, y);
        }

        // If the Joystick Z is pressed
        if (xSemaphoreTake(s4396122_SemaphoreJoystickZ, 1)) {

            // Send the new origin position to the mqtt stream
            char buffer[1024];
            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "(%d, %d)", -x - OS_DRAW_CANVAS_ORIGINAL_OFFSET_X, 
                    y - OS_DRAW_CANVAS_ORIGINAL_OFFSET_Y);
            s4396122_os_mqtt_publish("joystick", buffer);
            // Set the new origin position
            s4396122_os_draw_move_origin(-x - OS_DRAW_CANVAS_ORIGINAL_OFFSET_X, 
                    y - OS_DRAW_CANVAS_ORIGINAL_OFFSET_Y);
        }

        vTaskDelay(250);
    }
}

/**
 * @brief Task for handling the accelerometer input and setting the orientation
 */
void accel_Task() {

    // Used to make sure that the orientation isn't called constantly
    int lastVal; 

    while(1) {

        // Get the pl value
        struct PortLand pl = s4396122_hal_accel_get_pl();
        int currentVal; // Portrait or Landscape
        if (pl.landUp) {

            currentVal = OS_DRAW_PORTRAIT;
        } else {

            currentVal = OS_DRAW_LANDSCAPE;
        }

        // If the orientation has changed
        if (lastVal != currentVal) {

            // Update the orientation
            lastVal = currentVal;
            s4396122_os_draw_set_orientation(currentVal);

            // Send the status to the mqtt stream
            if (currentVal == OS_DRAW_PORTRAIT) {

                s4396122_os_mqtt_publish("accel", "portrait");
            } else {

                s4396122_os_mqtt_publish("accel", "landscape");
            }
        }
        
        vTaskDelay(100);
    }
}

/**
 * @brief Blinking LED Task to ensure a visual feedback that the system is 
 * alive
 */
void LED_Task() {

    while (1) {

        BRD_LEDToggle();
        vTaskDelay(250);
    }
}

/**
 * @brief The main function, this is called at system boot
 *
 * @return The exit code for the system
 */
int main() {

    BRD_init();
    Hardware_init(); // Initialise peripherials
    LwIP_Init();

    // Set the command handler for the mqtt input
    s4396122_os_mqtt_set_handler(mqtt_input); 

    // A series of structs containing all the relevant information for the 
    // tasks that are going to run
    struct TaskHolder LEDTask = {"LED", &LED_Task, mainLED_TASK_STACK_SIZE, 
        mainLED_PRIORITY};
    struct TaskHolder MainTask = {"Main", &main_Task, mainTask_TASK_STACK_SIZE, 
        mainTask_PRIORITY};
    struct TaskHolder DrawerTask = {"Drawer", &s4396122_DrawerTask, 
        drawerTask_TASK_STACK_SIZE, drawerTask_PRIORITY};
    struct TaskHolder IRTask = {"IR", &IR_Task, IRTask_TASK_STACK_SIZE, 
        IRTask_PRIORITY};
    struct TaskHolder JoystickTask = {"Joystick", &joystick_Task, 
        joystickTask_TASK_STACK_SIZE, joystickTask_PRIORITY};
    struct TaskHolder AccelTask = {"Accel", &accel_Task, 
        accelTask_TASK_STACK_SIZE, accelTask_PRIORITY};
    
    // Go through each struct just created and add to the array
    tasks[tasksPos++] = LEDTask;
    tasks[tasksPos++] = MainTask;
    tasks[tasksPos++] = DrawerTask;
    tasks[tasksPos++] = IRTask;
    tasks[tasksPos++] = JoystickTask;
    tasks[tasksPos++] = AccelTask;

    // Create all the referenced tasks
    for (int i = 0; i < tasksPos; i++) {
        xTaskCreate(tasks[i].function, tasks[i].name, tasks[i].stackDepth, 
                NULL, tasks[i].priority, NULL);
    }

    // initialize the mqtt library
    s4396122_os_mqtt_init();

    // MQTT Task reference information
    struct TaskHolder MQTTTask = {"MQTT", &MQTT_Task, mqttTask_STACK_SIZE, 
        mqttTask_PRIORITY};
    tasks[tasksPos++] = MQTTTask;

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

/**
 * @brief Displays the number of ticks the system has been running for
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvTimeCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {

    xWriteBufferLen = sprintf((char *) pcWriteBuffer, "Ticks: %d\n", 
            HAL_GetTick());
    return pdFALSE;
}

/**
 * @brief Displays the system status of the tasks for system
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvTopCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {

    // Store the task status in an array for accessing later
    unsigned long ulTotalRunTime;
    UBaseType_t uxArraySize = uxTaskGetNumberOfTasks();
    TaskStatus_t pxTaskStatusArray[uxArraySize];
    uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, 
            &ulTotalRunTime);
    ulTotalRunTime /= 100UL;

    char *pcWriteBufferPos = pcWriteBuffer;
    int bufferSize = 0;

    // Print out the remaining memory, number of tasks, and the first row of 
    // the table
    int writeSize = sprintf(pcWriteBufferPos, 
            "Remaining Memory: %d\nNumber of Tasks: 10\n   %-9s  %9s  %9s  %9s  %9s  %9s\n", 
            xPortGetMinimumEverFreeHeapSize(), "Name", "Priority", "Run Time", 
            "CPU Usage", "Stack Rem", "Status");
    pcWriteBufferPos += writeSize;
    bufferSize += writeSize;

    for (UBaseType_t i = 0; i < uxArraySize; i++) {
        unsigned long ulStatsAsPercentage = 
            pxTaskStatusArray[i].ulRunTimeCounter / ulTotalRunTime;
        // Get a string representation of the state
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

        int writeSize = sprintf(pcWriteBufferPos, 
                "%2d:%-9s  %9lu  %9lu  %8lu%%  %9u  %9s\n",
                pxTaskStatusArray[i].xTaskNumber,
                pxTaskStatusArray[i].pcTaskName,
                pxTaskStatusArray[i].uxCurrentPriority,
                pxTaskStatusArray[i].ulRunTimeCounter,
                ulStatsAsPercentage,
                pxTaskStatusArray[i].usStackHighWaterMark,
                stateName
                );
        // Increment the write buffer position
        pcWriteBufferPos += writeSize;
        bufferSize += writeSize;
    }

    xWriteBufferLen = bufferSize;

    return pdFALSE;
}

/**
 * @brief Suspends a task with the provided name
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return  If this command needs to run again
 */
static BaseType_t prvSuspendCommand(char *pcWriteBuffer, 
        size_t xWriteBufferLen, const char *pcCommandString) {

    // Get the task name
    long paramLen;
    const char *taskName = FreeRTOS_CLIGetParameter(pcCommandString, 1, 
            &paramLen);

    // Get the task handler from the task name
    TaskHandle_t taskHandle = xTaskGetHandle(taskName);
    if (taskHandle == NULL) {

        xWriteBufferLen = sprintf(pcWriteBuffer, "Invalid Task Name\n");
        return pdFALSE;
    }

    vTaskSuspend(taskHandle);

    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}

/**
 * @brief Resumes a task with the provided name
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvResumeCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {

    // Get the task name
    long paramLen;
    const char *taskName = FreeRTOS_CLIGetParameter(pcCommandString, 1, 
            &paramLen);

    // Get the task handler from the task name
    TaskHandle_t taskHandle = xTaskGetHandle(taskName);
    if (taskHandle == NULL) {

        xWriteBufferLen = sprintf(pcWriteBuffer, "Invalid Task Name\n");
        return pdFALSE;
    }

    vTaskResume(taskHandle);

    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}

/**
 * @brief Deletes a task with the provided name
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvDeleteCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {
    
    // Get the task name
    long paramLen;
    const char *taskName = FreeRTOS_CLIGetParameter(pcCommandString, 1, 
            &paramLen);

    // Get the task handler from the task name
    TaskHandle_t taskHandle = xTaskGetHandle(taskName);
    if (taskHandle == NULL) {

        xWriteBufferLen = sprintf(pcWriteBuffer, "Invalid Task Name\n");
        return pdFALSE;
    }

    vTaskDelete(taskHandle);

    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}

/**
 * @brief Creates a task with the provided name
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvCreateCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {

    // Get the task name
    long paramLen;
    const char *taskName = FreeRTOS_CLIGetParameter(pcCommandString, 1, 
            &paramLen);

    // Loop through all the tasks in the array and see if it matches the 
    // required task name
    for (int i = 0; i < tasksPos; i++) {
        if (strcmp(taskName, tasks[i].name) == 0) {
            xTaskCreate(tasks[i].function, tasks[i].name, tasks[i].stackDepth, 
                    NULL, tasks[i].priority, NULL);
            xWriteBufferLen = sprintf(pcWriteBuffer, "");
            return pdFALSE;
        }
    }

    xWriteBufferLen = sprintf(pcWriteBuffer, "Could not create task\n");
    return pdFALSE;
}

/**
 * @brief Adds a char to the display at the coords
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli paramters
 *
 * @return  If this command needs to run again
 */
static BaseType_t prvCharCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {

    // Get all the arguments in a big string
    long paramLen;
    const char *arguments = FreeRTOS_CLIGetParameter(pcCommandString, 1, 
            &paramLen);

    // Get the argument value from the passed arguments
    int x, y;
    char c;
    char d;
    sscanf(arguments, "%d %d %c", &x, &y, &c);

    // Convert the character to a segment mapping
    if (c >= 'a' && c <= 'z') {

        d = s4396122_os_draw_letter_to_segment[c - 'a'];
    } else if (c >= 'A' && c <= 'Z') {

        d = s4396122_os_draw_letter_to_segment[c - 'A'];
    } else if (c >= '0' && c <= '9') {

        d = s4396122_os_draw_number_to_segment[c - '0'];
    } else {

        xWriteBufferLen = sprintf(pcWriteBuffer, "Invalid character\n");
        return pdFALSE;
    }
    s4396122_os_draw_add_char(x, y, d);

    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}

/**
 * @brief Changes the colour of the pen
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvColourCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {

    // Get the pen char to change to
    long paramLen;
    const char *colour = FreeRTOS_CLIGetParameter(pcCommandString, 1, 
            &paramLen);

    // Convert char to enum item
    enum MouseColor c;
    switch (colour[0]) {

        case 'b':

            c = BLUE;
            break;

        case 'd':

            c = BLACK;
            break;

        case 'r':

            c = RED;
            break;

        case 'o':

            c = ORANGE;
            break;

        case 'w':

            c = WHITE;
            break;

        default:
            xWriteBufferLen = sprintf(pcWriteBuffer, "Invalid color\n");
            return pdFALSE;
    }
    s4396122_os_draw_change_pen_color(c);

    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}

/**
 * @brief Draws the string to screen
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvTextCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {

    // Get all the parameter in one string
    long paramLen;
    const char *arguments = FreeRTOS_CLIGetParameter(pcCommandString, 1, 
            &paramLen);

    // Convert the arguments string into the required variables
    int x, y;
    char str[12];
    sscanf(arguments, "%d %d %s", &x, &y, str);

    // For each character, convert to segment and add to display
    for (int i = 0; i < strlen(str); i++) {
        char d;
        if (str[i] >= 'a' && str[i] <= 'z')
            d = s4396122_os_draw_letter_to_segment[str[i] - 'a'];
        else if (str[i] >= 'A' && str[i] <= 'Z')
            d = s4396122_os_draw_letter_to_segment[str[i] - 'A'];
        else if (str[i] >= '0' && str[i] <= '9')
            d = s4396122_os_draw_number_to_segment[str[i] - '0'];
        else {
            xWriteBufferLen = sprintf(pcWriteBuffer, 
                    "Invalid Character at: %d\n", i);
            return pdFALSE;
        }
        s4396122_os_draw_add_char(x + i, y, d);
    }

    xWriteBufferLen = sprintf(pcWriteBuffer, "(%d, %d)\n", x, y);
    return pdFALSE;
}

/**
 * @brief Draws a polygon to the screen
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvPolyCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {

    // Get all the parameters into one string
    long paramLen;
    const char *arguments = FreeRTOS_CLIGetParameter(pcCommandString, 1, 
            &paramLen);

    // Convert the arguments string into the required variables
    int x, y, length, degree;
    sscanf(arguments, "%d %d %d %d", &x, &y, &length, &degree);

    s4396122_os_draw_add_poly(x, y, length, degree);

    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}

/**
 * @brief Removes the last drawn object from the screen
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvClearCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {

    s4396122_os_draw_remove_top();

    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}
