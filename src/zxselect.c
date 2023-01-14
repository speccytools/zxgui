
#include <string.h>
#include "zxgui.h"
#include "text_ui.h"
#include "zxgui_internal.h"

#define THIS_TYPE struct gui_select_t

#define COLOR (INK_YELLOW | BRIGHT | PAPER_BLACK)
#define COLOR_INV (INK_BLACK | BRIGHT | PAPER_YELLOW)

void _select_render()
{
    if (!is_object_invalidated())
        return;

    static struct gui_select_option_t** index;
    static uint8_t ww;
    static uint8_t i;
    static uint8_t selection_page;
    static uint8_t selection_page_offset;
    static uint8_t selection_offset;
    static uint8_t last_offset;

    index = (struct gui_select_option_t**) self()->obtain_data_cb();

    ww = this_basics.w - 1;

    selection_page = self()->selection / this_basics.h;
    selection_page_offset = selection_page * this_basics.h;

    selection_offset = self()->selection - selection_page_offset;
    last_offset = self()->last_selection - selection_page_offset;

    if (selection_page != (self()->last_selection / this_basics.h))
    {
        // different pages
        this_flags |= GUI_FLAG_DIRTY;
    }

    if (this_flags & GUI_FLAG_DIRTY)
    {
        zxgui_screen_color(COLOR);
        zxgui_screen_clear(this_basics.x, this_basics.y, this_basics.w, this_basics.h);

        static uint8_t offset;
        static uint8_t y_offset;
        static uint8_t options_size;

        for (i = selection_page_offset, offset = 0, y_offset = this_basics.y, options_size = self()->options_size
            ; offset < this_basics.h && i < options_size; i++, offset++, y_offset++)
        {
            struct gui_select_option_t* o = index[i];

            static uint8_t this_w;
            static uint8_t this_x;
            static uint8_t len;

            this_w = this_basics.w;
            this_x = this_basics.x;

            if (o->icon)
            {
                this_w--;
                this_x++;
                zxgui_image(this_basics.x, y_offset, 1, 1, o->icon, &o->icon_color);
            }

            if (i == self()->selection)
            {
                text_ui_color(COLOR_INV);
                zxgui_screen_color(COLOR_INV);
                zxgui_screen_clear(this_x, y_offset, this_w, 1);
            }
            else
            {
                zxgui_screen_color(COLOR);
                text_ui_color(COLOR);
            }

            len = strlen(o->value);

            if (len > (this_w << 1)) len = this_w << 1;
            text_ui_write_at(this_x, y_offset, o->value, len);

            if ((offset == 0) && (i != 0))
            {
                zxgui_screen_put(this_basics.x + ww, y_offset, GUI_ICON_LESS_TO_FOLLOW);
            }
            else if (offset == this_basics.h - 1 && (i < options_size))
            {
                zxgui_screen_put(this_basics.x + ww, y_offset, GUI_ICON_MORE_TO_FOLLOW);
            }
        }
    }

    if (this_flags & GUI_FLAG_DIRTY_INTERNAL)
    {
        static uint8_t yy_offset;

        yy_offset = this_basics.y + (selection_offset % this_basics.h);

        zxgui_screen_color(COLOR_INV);
        if (index[self()->selection]->icon)
        {
            zxgui_screen_recolor(this_basics.x + 1, yy_offset, ww, 1);
        }
        else
        {
            zxgui_screen_recolor(this_basics.x, yy_offset, this_basics.w, 1);
        }
        if (last_offset != 0xFF)
        {
            yy_offset = this_basics.y + (last_offset % this_basics.h);

            zxgui_screen_color(COLOR);
            if (index[self()->last_selection]->icon)
            {
                zxgui_screen_recolor(this_basics.x + 1, yy_offset, ww, 1);
            }
            else
            {
                zxgui_screen_recolor(this_basics.x, yy_offset, this_basics.w, 1);
            }
        }
    }

    object_validate();
}

void zxgui_select_change_option(struct gui_select_t* select, uint8_t i) ZXGUI_CDECL
{
    if (select->selection == i)
    {
        return;
    }
    select->last_selection = select->selection;
    select->selection = i;
    select->base.flags |= GUI_FLAG_DIRTY_INTERNAL;
    struct gui_select_option_t** index = (struct gui_select_option_t**) select->obtain_data_cb();
    select->selected(index[i]);
}

uint8_t _select_event(enum gui_event_type event_type, void* event)
{
    if (current_scene->focus != (void*)this)
        return 0;

    switch (event_type)
    {
        case GUI_EVENT_KEY_PRESSED:
        {
            struct gui_event_key_pressed* ev = event;

            switch (ev->key)
            {
                case GUI_KEY_CODE_UP:
                {
                    if (self()->selection)
                    {
                        zxgui_select_change_option(self(), self()->selection - 1);
                        object_invalidate(GUI_FLAG_DIRTY_INTERNAL);
                    }
                    break;
                }
                case GUI_KEY_CODE_DOWN:
                {
                    if (self()->selection < self()->options_size - 1)
                    {
                        zxgui_select_change_option(self(), self()->selection + 1);
                        object_invalidate(GUI_FLAG_DIRTY_INTERNAL);
                    }
                    break;
                }
            }

            /*
            if (isalnum(ev->key))
            {
                struct gui_select_option_t* f = self()->first;

                if (self()->selection && strchr(self()->selection->value, ev->key) == self()->selection->value)
                {
                    f = self()->selection->next;
                    if (f == NULL || strchr(f->value, ev->key) != f->value)
                    {
                        f = self()->first;
                    }
                }

                for (struct gui_select_option_t* it = f; it; it = it->next)
                {
                    if (strchr(it->value, ev->key) == it->value)
                    {
                        select_change_selected_option(it);
                        break;
                    }
                }
            }
             */

            break;
        }
    }

    return 0;
}

uint8_t* zxgui_select_add_option(struct gui_select_t* select, const char* option,
    uint8_t option_len, uint16_t user_data_amount, uint8_t* icon, uint8_t icon_color) ZXGUI_CDECL
{
    if (select->options_size == select->options_capacity)
    {
        return NULL;
    }

    uint8_t* buffer = select->obtain_data_cb();
    struct gui_select_option_t** index = (struct gui_select_option_t**)buffer;

    uint16_t needed = sizeof(struct gui_select_option_t) + option_len + 1 + user_data_amount;

    struct gui_select_option_t* o = (struct gui_select_option_t*)(buffer + select->buffer_offset);
    index[select->options_size++] = o;
    select->buffer_offset += needed;

    memcpy(o->value, option, option_len);
    o->value[option_len] = 0;
    o->user = user_data_amount ? o->value + option_len + 1 : NULL;
    memcpy(o->icon, icon, 8);
    o->icon_color = icon_color;

    return o->user;
}