#include <string.h>
#include "zxgui_internal.h"
#include "zxgui.h"

void zxgui_screen_put(uint8_t x, uint8_t y, uint8_t ch) __z88dk_callee
{
    _c = zxgui_tiles + ch * 8;
    _addr = zx_cxy2saddr(x, y);

    for (_i = 0; _i < 8; _i++)
    {
        *_addr = *_c++;
        _addr += 256;
    }

    *zx_cxy2aaddr(x, y) = screen_color;
}

void zxgui_screen_clear(uint8_t x, uint8_t y, uint8_t w, uint8_t h) __z88dk_callee
{
    _c = zx_cxy2aaddr(x, y);

    for (_i = y, _j = y + h; _i < _j; _i++)
    {
        _addr = zx_cxy2saddr(x, _i);

        for (uint8_t i = 0; i < 8; i++)
        {
            memset(_addr, 0, w);
            _addr += 256;
        }

        memset(_c, screen_color, w);
        _c += 32;
    }
}

void zxgui_screen_recolor(uint8_t x, uint8_t y, uint8_t w, uint8_t h) __z88dk_callee
{
    _c = zx_cxy2aaddr(x, y);

    for (_i = y, _j = y + h; _i < _j; _i++)
    {
        memset(_c, screen_color, w);
        _c += 32;
    }
}