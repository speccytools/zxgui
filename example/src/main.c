#include <string.h>
#include <arch/zx/spectrum.h>
#include "zxgui.h"

#include "welcome.inc.h"

static void on_next_pressed(void)
{
    strcpy(hello_username, "Hello, ");
    strcat(hello_username, username);
    strcat(hello_username, "!");

    zxgui_clear();

    const char* things_to_like[] = {
        "dogs",
        "cats",
        "hamsters",
        "horses",
        "pigs",
        "scolopendras",
        NULL
    };

    for (const char** ref = things_to_like; *ref; ref++)
    {
        zxgui_select_add_option(&what_to_like, *ref, strlen(*ref), 0, NULL, 0);
    }

    zxgui_scene_set(&likes);
    zxgui_select_change_option(&what_to_like, 1);
}

static uint8_t* get_like_data(void)
{
    static uint8_t select_data[256];
    return select_data;
}

static void on_like_selected(struct gui_select_option_t* selected)
{
    strcpy(i_like_what, "I like ");
    strcat(i_like_what, selected->value);
    zxgui_set_dirty(i_like);
}

static void proceed_with_like(void)
{
    zxgui_clear();
    zxgui_scene_set(&picture);
}

int main()
{
    zxgui_clear();
    zxgui_scene_set(&welcome);
    // can be also specified by setting "focus: true" on .yaml
    zxgui_scene_set_focus(&welcome, &user_name_edit);

    while (1)
    {
        zxgui_scene_iteration();
#asm
        halt
#endasm
    }
}