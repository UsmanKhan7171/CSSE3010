#ifndef OS_DRAW_H
#define OS_DRAW_H

char s4396122_os_draw_number_to_segment[10] = {
    0x77,   // 0
    0x22,   // 1
    0x5B,   // 2
    0x6B,   // 3
    0x2E,   // 4
    0x6D,   // 5
    0x7D,   // 6
    0x27,   // 7
    0x7F,   // 8
    0x6F,   // 9
};

char s4396122_os_draw_letter_to_segment[26] = {
    0x3F,   // a
    0x7C,   // b
    0x58,   // c
    0x7A,   // d
    0x5D,   // e
    0x1D,   // f
    0x6F,   // g
    0x3C,   // h
    0x22,   // i
    0x72,   // j
    0x1C,   // k
    0x34,   // l
    0x38,   // m
    0x38,   // n
    0x78,   // o
    0x1F,   // p
    0x2F,   // q
    0x18,   // r
    0x6D,   // s
    0x15,   // t
    0x70,   // u
    0x30,   // v
    0x70,   // w
    0x48,   // x
    0x6E,   // y
    0x5B,   // z
}

struct drawChar {
    int x;
    int y;
    char c;
}

#endif
