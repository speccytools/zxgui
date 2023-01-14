#ifndef ZXGUI_INTERNAL
#define ZXGUI_INTERNAL

#include <stdint.h>
#include <spectrum.h>
#include <input.h>

struct gui_edit_t;

#define SCREEN_WIDTH (32)
#define SCREEN_HEIGHT (24)
#define CHARACTERS_PER_CELL (2)
#define BLINK_INTERVAL (10)

#define INK_BLACK      0x00
#define INK_BLUE       0x01
#define INK_RED        0x02
#define INK_MAGENTA    0x03
#define INK_GREEN      0x04
#define INK_CYAN       0x05
#define INK_YELLOW     0x06
#define INK_WHITE      0x07

#define PAPER_BLACK    0x00
#define PAPER_BLUE     0x08
#define PAPER_RED      0x10
#define PAPER_MAGENTA  0x18
#define PAPER_GREEN    0x20
#define PAPER_CYAN     0x28
#define PAPER_YELLOW   0x30
#define PAPER_WHITE    0x38

#define BRIGHT         0x40
#define FLASH          0x80

extern void _render_blink_even(uint8_t *addr) __z88dk_fastcall __naked;
extern void _render_blink_odd(uint8_t *addr) __z88dk_fastcall __naked;
extern void clear_blink(struct gui_edit_t* this) __z88dk_fastcall;

extern uint8_t _j;
extern uint8_t _i;
extern uint8_t* _c;
extern uint8_t* _addr;
extern unsigned char zxgui_tiles[];

#endif