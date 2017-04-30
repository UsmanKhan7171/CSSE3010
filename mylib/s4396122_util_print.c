#include "s4396122_util_print.h"

void s4396122_util_print_move(int x, int y) {
    debug_printf("\033[%d;%dH", x, y);
}

void s4396122_util_print_clear() {
    debug_printf("\033[2J");
}

void s4396122_util_print_color(int sgr) {
    debug_printf("\033[%dm", sgr);
}

void s4396122_util_print_reset() {
    s4396122_util_print_color(0);
}

void s4396122_util_print_bold() {
    s4396122_util_print_color(1);
}

void s4396122_util_print_reverse() {
    s4396122_util_print_color(7);
}

void error(const char *s, const char *fileName, int lineNo) {
    s4396122_util_print_color(s4396122_util_print_red);
    s4396122_util_print_bold();
    debug_printf("%s - %s:%d\n", s, fileName, lineNo);
    s4396122_util_print_reset();
}

void warn(const char *s, const char *fileName, int lineNo) {
    s4396122_util_print_color(s4396122_util_print_yellow);
    s4396122_util_print_bold();
    debug_printf("%s - %s:%d\n", s, fileName, lineNo);
    s4396122_util_print_reset();
}

void info(const char *s, const char *fileName, int lineNo) {
    s4396122_util_print_color(s4396122_util_print_blue);
    s4396122_util_print_bold();
    debug_printf("[%s:%d] %s\n", fileName, lineNo, s);
    s4396122_util_print_reset();
}

void debug(const char *s, const char *fileName, int lineNo) {
    #ifdef SDEBUG
        debug_printf("[%s:%d] %s\n", fileName, lineNo, s);
    #endif
}

void print(int errorType, const char *fileName, int lineNo, const char *format, ...) {
    va_list valist;
    char buffer[50];
    vsprintf(&buffer, format, valist);
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
