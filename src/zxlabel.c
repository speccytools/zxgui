
#include <string.h>
#include "zxgui.h"
#include "text_ui.h"

#define THIS_TYPE struct gui_label_t

void _label_render()
{
    if (!is_object_invalidated())
        return;

    object_validate();

    if (this_flags & GUI_FLAG_HIDDEN)
    {
        return;
    }

    static char* c;
    c = (char*)self()->title;
    if (*c == '\0')
        return;

    zxgui_screen_color(self()->color);
    zxgui_screen_clear(this_basics.x, this_basics.y, this_basics.w, this_basics.h);
    text_ui_color(self()->color);

    if (this_flags & GUI_FLAG_MULTILINE)
    {
        const char* end = c + strlen(c);
        uint8_t offset_x = this_basics.x;
        uint8_t offset_y = this_basics.y;
        uint8_t max_offset_y = offset_y + this_basics.h;

        do
        {
            while (*c == ' ')
            {
                // skip spaces at beginning of the line
                c++;
            }

            char* next_line = text_ui_buffer_partition(c, (uint16_t) (end - c), this_basics.w);

            if (c != next_line)
            {
                text_ui_write_at(offset_x, offset_y, c, next_line - c);
            }

            if (*next_line == '\n')
            {
                next_line++;
            }

            offset_y++;
            if (offset_y >= max_offset_y)
            {
                break;
            }
            c = next_line;
        } while (c < end);
    }
    else
    {
        text_ui_puts_at(this_basics.x, this_basics.y, self()->title);
    }
}

uint8_t zxgui_label_text_height(uint8_t w, const char* title, uint16_t len, uint8_t max_height) ZXGUI_CDECL
{
    char* c = (char*)title;
    const char* end = c + len;
    uint8_t res = 0;

    do
    {
        while (*c == ' ')
        {
            // skip spaces at beginning of the line
            c++;
        }

        char* next_line = text_ui_buffer_partition(c, (uint16_t) (end - c), w);

        if (next_line == c)
        {
            next_line = memchr(c, '\n', (uint16_t)(end - c));
            if (next_line == NULL)
            {
                return 3;
            }
        }

        if (*next_line == '\n')
        {
            next_line++;
        }

        res++;
        c = next_line;

        if (res > max_height)
        {
            return max_height;
        }
    } while (c < end);

    return res;
}
