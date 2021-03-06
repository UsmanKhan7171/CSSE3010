/**
 * @file s4396122_cli_draw.h
 * @brief Provides drawing commands for CLI
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef CLI_DRAW_H
#define CLI_DRAW_H

#include "s4396122_os_draw.h"
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"

static BaseType_t prvMoveCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString);
static BaseType_t prvPenDownCommand(char *pcWriteBuffer, 
        size_t xWriteBufferLen, const char *pcCommandString);
static BaseType_t prvPenUpCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString);
static BaseType_t prvBoxCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString);
static BaseType_t prvLineCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString);
static BaseType_t prvResetCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString);
static BaseType_t prvPenCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString);
static BaseType_t prvRotateCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString);

void s4396122_cli_draw_init();

#endif
