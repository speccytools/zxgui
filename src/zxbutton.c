
#include "zxgui.h"
#include <arch/zx/spectrum.h>
#include "text_ui.h"

#define THIS_TYPE struct gui_button_t

void _button_render()
{
    static uint8_t x;
    x = this_basics.x;

    if (this_flags & GUI_FLAG_HIDDEN)
    {
        return;
    }

    if (is_object_invalidated())
    {
        object_validate();
        zxgui_screen_color(INK_BLACK | PAPER_BLACK);
        zxgui_screen_clear(this_basics.x, this_basics.y, this_basics.w, this_basics.h);

        zxgui_screen_color(INK_GREEN | BRIGHT | PAPER_BLACK);

        if (this_flags & GUI_FLAG_SYM)
        {
            zxgui_screen_put(x, this_basics.y, GUI_ICON_SYM);
            x++;
        }

        zxgui_screen_put(x, this_basics.y, self()->icon);

        if (self()->title)
        {
            x++;

            text_ui_color(INK_WHITE | PAPER_BLACK);
            text_ui_puts_at(x, this_basics.y, (char *) self()->title);
        }
    }
}

extern uint8_t is_alt_key_pressed();

uint8_t _button_event(enum gui_event_type event_type, void* event)
{
    switch (event_type)
    {
        case GUI_EVENT_KEY_PRESSED:
        {
            struct gui_event_key_pressed* ev = event;
            if (ev->key == self()->key)
            {
                if (this_flags & GUI_FLAG_SYM)
                {
                    if (is_alt_key_pressed() == 0)
                    {
                        break;
                    }
                }
                if (self()->pressed)
                {
                    self()->pressed();
                }
                return 1;
            }

            break;
        }
    }
    return 0;
}