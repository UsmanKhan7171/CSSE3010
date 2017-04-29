#ifndef UTIL_PRINT_H
#define UTIL_PRINT_H

#include <debug_printf.h>
#include <stdarg.h>

#define DEBUG 1

#define s4396122_util_print_black 30
#define s4396122_util_print_red 31
#define s4396122_util_print_green 32
#define s4396122_util_print_yellow 33
#define s4396122_util_print_blue 34
#define s4396122_util_print_purple 35
#define s4396122_util_print_teal 36
#define s4396122_util_print_white 37

#define s4396122_util_print_black_background 30
#define s4396122_util_print_red_background 31
#define s4396122_util_print_green_background 32
#define s4396122_util_print_yellow_background 33
#define s4396122_util_print_blue_background 34
#define s4396122_util_print_purple_background 35
#define s4396122_util_print_teal_background 36
#define s4396122_util_print_white_background 37

#define s4396122_util_print_error(s) error(s, __FILE__, __LINE__)
#define s4396122_util_print_warn(s) warn(s, __FILE__, __LINE__)
#define s4396122_util_print_info(s) info(s, __FILE__, __LINE__)
#define s4396122_util_print_debug(s) debug(s, __FILE__, __LINE__)
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

void print(int errorType, const char *fileName, int lineNo, const char *format, ...);

#endif
