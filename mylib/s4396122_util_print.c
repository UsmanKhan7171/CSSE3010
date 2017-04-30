/**
 * @file s4396122_util_print.c
 * @brief Library for handling serial output and cursor magic
 * @author Daniel Fitzmaurice - 43961229
 * @version 1.0
 * @date 2017-04-30
 */
#include "s4396122_util_print.h"

/**
 * @brief Moves the cursor to (x, y)
 *
 * @param x position on the x to move the cursor to
 * @param y position on the y to move the cursor to
 */
void s4396122_util_print_move(int x, int y) {
    debug_printf("\033[%d;%dH", x, y);
}

/**
 * @brief Clears the serial screen
 */
void s4396122_util_print_clear() {
    debug_printf("\033[2J");
}

/**
 * @brief Changes the terminal color, either foreground or background
 *
 * @param sgr The color to set to
 */
void s4396122_util_print_color(int sgr) {
    debug_printf("\033[%dm", sgr);
}

/**
 * @brief Resets the cursor back to default
 */
void s4396122_util_print_reset() {
    s4396122_util_print_color(0);
}

/**
 * @brief Bolds the cursor
 */
void s4396122_util_print_bold() {
    s4396122_util_print_color(1);
}

/**
 * @brief Inverts the cursor color
 */
void s4396122_util_print_reverse() {
    s4396122_util_print_color(7);
}

/**
 * @brief Prints an error to the screen
 *
 * @param s String to print to the screen
 * @param fileName Macro of origin of the error
 * @param lineNo Macro of origin of the error
 */
void error(const char *s, const char *fileName, int lineNo) {
    s4396122_util_print_color(s4396122_util_print_red);
    s4396122_util_print_bold();
    debug_printf("%s - %s:%d\n", s, fileName, lineNo);
    s4396122_util_print_reset();
}

/**
 * @brief Prints a warning to the screen
 *
 * @param s String to print to the screen
 * @param fileName Macro of the origin of the warning
 * @param lineNo Macro of the origin of the warning
 */
void warn(const char *s, const char *fileName, int lineNo) {
    s4396122_util_print_color(s4396122_util_print_yellow);
    s4396122_util_print_bold();
    debug_printf("%s - %s:%d\n", s, fileName, lineNo);
    s4396122_util_print_reset();
}

/**
 * @brief Prints an info statement to the screen
 *
 * @param s String to print to the screen
 * @param fileName Macro of the origin of the info
 * @param lineNo Macro of the origin of the info
 */
void info(const char *s, const char *fileName, int lineNo) {
    s4396122_util_print_color(s4396122_util_print_blue);
    s4396122_util_print_bold();
    debug_printf("[%s:%d] %s\n", fileName, lineNo, s);
    s4396122_util_print_reset();
}

/**
 * @brief Prints a debug statement to the screen
 *
 * @param s String to print to the screen
 * @param fileName Macro of the origin of the debug
 * @param lineNo Macro of the origin of the debug
 */
void debug(const char *s, const char *fileName, int lineNo) {
    #ifdef SDEBUG
        debug_printf("[%s:%d] %s\n", fileName, lineNo, s);
    #endif
}

/**
 * @brief Prints a error/warn/info/debug to the screen based on the errorType
 *
 * @param errorType The level of threat the message poses, (0=error, 3=debug)
 * @param fileName Macro of the origin of the message
 * @param lineNo Macro of the origin of the message
 * @param format Supports printf style input formatting
 * @param ...
 */
void print(int errorType, const char *fileName, int lineNo, const char *format, ...) {
    va_list valist;
    char buffer[50];
    va_start(valist, format);
    vsprintf(buffer, format, valist);
    va_end(valist);
    switch (errorType) {
        case 0:
            error(buffer, fileName, lineNo);
            break;
        case 1:
            warn(buffer, fileName, lineNo);
            break;
        case 2:
            info(buffer, fileName, lineNo);
            break;
        default:
            debug(buffer, fileName, lineNo);
            break;
    }
    debug_flush();
}
