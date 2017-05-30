/**
 * @file s4396122_util_print.h
 * @brief Function for handling the serial print and cursor magic
 * @author Daniel Fitzmaurice - 43961229
 * @version 1.0
 * @date 2017-04-30
 */
#ifndef UTIL_PRINT_H
#define UTIL_PRINT_H

#include <stdarg.h>
#include "s4396122_os_print.h"

#define s4396122_util_print_black 30    //!< Set the foreground color to black
#define s4396122_util_print_red 31      //!< Set the foreground color to red
#define s4396122_util_print_green 32    //!< Set the foreground color to green
#define s4396122_util_print_yellow 33   //!< Set the foreground color to yellow
#define s4396122_util_print_blue 34     //!< Set the foreground color to blue
#define s4396122_util_print_purple 35   //!< Set the foreground color to purple
#define s4396122_util_print_teal 36     //!< Set the foreground color to teal
#define s4396122_util_print_white 37    //!< Set the foreground color to white

//! Set background to black
#define s4396122_util_print_black_background 30
//! Set background to red
#define s4396122_util_print_red_background 31
//! Set background to green
#define s4396122_util_print_green_background 32
//! Set background to yellow
#define s4396122_util_print_yellow_background 33
//! Set background to blue
#define s4396122_util_print_blue_background 34
//! Set background to purple
#define s4396122_util_print_purple_background 35
//! Set background to teal
#define s4396122_util_print_teal_background 36
//! Set background to white
#define s4396122_util_print_white_background 37

//! Print out an error message with the correct fileName and lineNo
#define s4396122_util_print_error(s) error(s, __FILE__, __LINE__)
//! Print out a warning message with the correct fileName and lineNo
#define s4396122_util_print_warn(s) warn(s, __FILE__, __LINE__)
//! Print out an info message with the correct fileName and lineNo
#define s4396122_util_print_info(s) info(s, __FILE__, __LINE__)
//! Print out a debug message with the correct fileName and lineNo
#define s4396122_util_print_debug(s) debug(s, __FILE__, __LINE__)
//! Print out a message with the correct fileName and lineNo
#define s4396122_util_print(s, f, args...) print(s, __FILE__, __LINE__, f, args)

extern void s4396122_util_print_move(int x, int y);
extern void s4396122_util_print_clear();
extern void s4396122_util_print_color(int sgr);
extern void s4396122_util_print_reset();
extern void s4396122_util_print_bold();
extern void s4396122_util_print_reverse();

void error(const char *s, const char *fileName, int lineNo);
void warn(const char *s, const char *fileName, int lineNo);
void info(const char *s, const char *fileName, int lineNo);
void debug(const char *s, const char *fileName, int lineNo);

void print(int errorType, const char *fileName, int lineNo, const char *format, 
        ...);

#endif
