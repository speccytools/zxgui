#include "zxgui.h"
#include "zxgui_internal.h"

enum input_state_t
{
    waiting,
    key_pressed,
    key_pressed_w_sym,
};

static enum input_state_t input_state = waiting;
static char last_key_pressed = 0;

uint8_t is_alt_key_pressed()
{
    return in_KeyPressed(0x4000);
}

void update_keyboard()
{
    switch (input_state)
    {
        case waiting:
        {
            last_key_pressed = (char)in_Inkey();
            if (last_key_pressed)
            {
                struct gui_event_key_pressed e;
                e.key = last_key_pressed;
                zxgui_scene_dispatch_event(GUI_EVENT_KEY_PRESSED, &e);

                if (in_KeyPressed(0x4000) == 0 && in_KeyPressed(0x2000) == 0)
                {
                    input_state = key_pressed;
                }
                else
                {
                    input_state = key_pressed_w_sym;
                }
            }
            break;
        }
        case key_pressed_w_sym:
        {
            if (0 == (char)in_Inkey())
            {
                input_state = waiting;
            }
            break;
        }
        case key_pressed:
        {
            char key = (char)in_Inkey();
            if (key != last_key_pressed)
            {
                input_state = waiting;
            }
            break;
        }
    }
}