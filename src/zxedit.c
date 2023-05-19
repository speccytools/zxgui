
#include <string.h>
#include "zxgui.h"
#include "text_ui.h"
#include "zxgui_internal.h"

#define THIS_TYPE struct gui_edit_t

static uint8_t blink = 0;

static void set_cursor_addresses(struct gui_edit_t* this, uint8_t x, uint8_t y) __z88dk_callee
{
    this->cursor_pixels_addr = zx_cxy2saddr(x, y);
    this->cursor_color_addr = zx_cxy2aaddr(x, y);
}

void _edit_render()
{
    // optimizing code footprint
    static uint8_t offset_y;
    static uint8_t last_line_len;
    static uint8_t max_offset_y;
    static const char* end;
    static char* c;
    static char* next_line;
    static uint8_t xx;
    static uint8_t half_w;
    static uint8_t w;
    static uint8_t col;

    if (is_object_invalidated())
    {
        object_validate();

        col = (current_scene->focus == NULL || current_scene->focus == this) ? (BRIGHT | INK_YELLOW | PAPER_BLACK) : (BRIGHT | INK_WHITE | PAPER_BLACK);

        zxgui_screen_color(PAPER_BLACK | INK_BLACK);
        text_ui_color(col);

        if ((this_flags & GUI_FLAG_MULTILINE) && self()->value[0])
        {
            c = (char*)self()->value;
            end = c + strlen(c);
            max_offset_y = this_basics.y + this_basics.h;

            offset_y = this_basics.y;
            last_line_len = 0;

            do
            {
                next_line = text_ui_buffer_partition(c, (uint16_t) (end - c), this_basics.w);

                if (c != next_line)
                {
                    last_line_len = next_line - c;
                    uint8_t text_l = (last_line_len >> 1) + (last_line_len & 0x01);
                    zxgui_screen_clear(this_basics.x + text_l, offset_y, this_basics.w - text_l, 1);
                    text_ui_write_at(this_basics.x, offset_y, c, last_line_len);
                }

                if (*next_line == '\n')
                {
                    next_line++;
                    last_line_len = 0;
                }

                offset_y++;
                if (offset_y >= max_offset_y)
                {
                    break;
                }
                c = next_line;
            } while (c < end);

            xx = this_basics.x + (last_line_len >> 1);
            if (offset_y < max_offset_y)
            {
                zxgui_screen_clear(this_basics.x, offset_y, this_basics.w, 1);
            }

            if (last_line_len != 0)
            {
                offset_y--;
            }

            self()->cursor_even = (last_line_len & 0x01);
            set_cursor_addresses(self(), xx, offset_y);
        }
        else
        {
            w = strlen((char *) self()->value);
            half_w = (w >> 1);
            xx = this_basics.x + half_w;

            zxgui_screen_clear(xx, this_basics.y, this_basics.w - half_w, this_basics.h);
            if (self()->value[0])
            {
                text_ui_write_at(this_basics.x, this_basics.y, self()->value, w);
            }
            self()->cursor_even = (w & 0x01);
            set_cursor_addresses(self(), xx, this_basics.y);
        }
        blink = BLINK_INTERVAL;
    }

    if (current_scene->focus == NULL || current_scene->focus == this)
    {
        if (blink++ > BLINK_INTERVAL)
        {
            blink = 0;

            if (self()->cursor_even)
            {
                _render_blink_even(self()->cursor_pixels_addr);
            }
            else
            {
                _render_blink_odd(self()->cursor_pixels_addr);
            }

            (*self()->cursor_color_addr) = INK_YELLOW | BRIGHT | PAPER_BLACK;
        }
    }
}

uint8_t _edit_event(enum gui_event_type event_type, void* event)
{
    if (current_scene->focus != NULL && current_scene->focus != this)
        return 0;

    switch (event_type)
    {
        case GUI_EVENT_KEY_PRESSED:
        {
            struct gui_event_key_pressed* ev = event;
            char key = ev->key;

            switch (key)
            {
                // backspace
                case GUI_KEY_CODE_BACKSPACE:
                {
                    int len = strlen(self()->value);
                    if (len)
                    {
                        clear_blink(self());
                        len--;
                        self()->value[len] = 0;
                        blink = BLINK_INTERVAL;

                        object_invalidate(GUI_FLAG_DIRTY_INTERNAL);
                    }
                    return 1;
                }
                // enter
                case GUI_KEY_CODE_RETURN:
                {
                    if ((this_flags & GUI_FLAG_MULTILINE) == 0)
                    {
                        return 0;
                    }
                    else
                    {
                        if (zxgui_label_text_height(this_basics.w - 1, self()->value, strlen(self()->value), this_basics.h)
                                >= this_basics.h - 2)
                        {
                            return 0;
                        }
                    }
                    /* fall through */
                    key = '\n';
                }
                default:
                {
                    int len = strlen(self()->value);
                    if (len >= self()->value_size - 1)
                    {
                        return 0;
                    }
                    if ((this_flags & GUI_FLAG_MULTILINE) == 0)
                    {
                        if (len >= ((this_basics.w - 1) << 1))
                        {
                            return 0;
                        }
                    }

                    clear_blink(self());

                    self()->value[len] = key;
                    len++;
                    self()->value[len] = 0;
                    blink = BLINK_INTERVAL;

                    object_invalidate(GUI_FLAG_DIRTY_INTERNAL);

                    return 1;
                }
            }
        }
    }

    return 0;
}
