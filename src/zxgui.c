#include "zxgui.h"
#include "zxgui_internal.h"

#include <string.h>

uint8_t screen_color = 0;
uint8_t _j;
uint8_t _i;
uint8_t* _c;
uint8_t* _addr;

void zxgui_clear(void)
{
    zx_border(INK_BLACK);
    zx_colour(BRIGHT | INK_GREEN | PAPER_BLACK);
    zx_cls();
}

void zxgui_image(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t* source, const uint8_t* colors) __z88dk_callee
{
    for (_j = y; _j < y + h; _j++)
    {
        for (_i = x; _i < x + w; _i++)
        {
            uint8_t* addr = zx_cxy2saddr(_i, _j);

            for (uint8_t ii = 0; ii < 8; ii++)
            {
                *addr = *source++;
                addr += 256;
            }

            *zx_cxy2aaddr(_i, _j) = *colors++;
        }
    }
}
