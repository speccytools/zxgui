#ifndef ZXGUI_H
#define ZXGUI_H

#define ZXGUI_CDECL __z88dk_callee __stdc

#include <stdint.h>

enum gui_tiles {
    GUI_STRIPE = 0,
    GUI_ICON_RETURN,
    GUI_ICON_C,
    GUI_ICON_LOADING_A_1,
    GUI_ICON_LOADING_A_2,
    GUI_ICON_LOADING_A_3,
    GUI_ICON_LOADING_A_4,
    GUI_ICON_LOADING_B_1,
    GUI_ICON_LOADING_B_2,
    GUI_ICON_LOADING_B_3,
    GUI_ICON_LOADING_B_4,
    GUI_ICON_SPACE,
    GUI_ICON_MORE_TO_FOLLOW,
    GUI_ICON_LESS_TO_FOLLOW,
    GUI_ICON_P,
    QUESTION_MARK_1,
    QUESTION_MARK_2,
    QUESTION_MARK_3,
    QUESTION_MARK_4,
    GUI_ICON_6,
    GUI_ICON_7,
    GUI_ICON_LOADING_SMALL_1,
    GUI_ICON_LOADING_SMALL_2,
    GUI_SELECTED_ENTRY,
    GUI_ICON_REPLIES,
    GUI_ICON_REPLY,
    GUI_ICON_H,
    GUI_ICON_R,
    GUI_ICON_N,
    GUI_ICON_SYM,
    GUI_ICON_5,
    GUI_ICON_T,
    GUI_ICON_B,
    GUI_PROGRESS_LEFT,
    GUI_PROGRESS_MIDDLE,
    GUI_PROGRESS_RIGHT,
    GUI_PROGRESS_FILLED_LEFT,
    GUI_PROGRESS_FILLED_MIDDLE,
    GUI_PROGRESS_FILLED_RIGHT,
    HEALTH_4,
    HEALTH_3,
    HEALTH_2,
    HEALTH_1,
    POWER_4,
    POWER_3,
    POWER_2,
    POWER_1
};

enum gui_event_type {
    GUI_EVENT_KEY_PRESSED,
};

enum gui_event_keycodes {
    GUI_KEY_CODE_DOWN = 10,
    GUI_KEY_CODE_UP = 11,
    GUI_KEY_CODE_BACKSPACE = 12,
    GUI_KEY_CODE_RETURN = 13,
    GUI_KEY_CODE_ESCAPE = 27,
};

struct gui_event_key_pressed
{
    char key;
};

struct gui_object_t;
struct gui_scene_t;

extern struct gui_object_t* this;
extern uint8_t this_flags;
extern struct gui_object_basics_t this_basics;

#define self() ((THIS_TYPE*)this)
#define base(a) a->base

typedef void (*gui_render_f)(void);
typedef uint8_t (*gui_event_f)(enum gui_event_type event_type, void* event);

#define GUI_FLAG_DIRTY (0x01u)
#define GUI_FLAG_DIRTY_INTERNAL (0x02u)
#define GUI_FLAG_MULTILINE (0x04u)
#define GUI_FLAG_HIDDEN (0x08u)
#define GUI_FLAG_SYM (0x10u)
#define GUI_FLAGS_CLEAR_DIRTY (0xFC)

struct gui_object_basics_t
{
    uint8_t x;
    uint8_t y;
    uint8_t w;
    uint8_t h;
};

struct gui_object_t
{
    struct gui_object_basics_t basics;
    uint8_t flags;
    struct gui_object_t* next;
    gui_render_f render;
    gui_event_f event;
};

#define base_ptr(x) &x.base

#define zxgui_base_init(next, x, y, w, h, render, event, flags) \
    { {x, y, w, h}, flags, next, render, event}

#define zxgui_set_dirty(o) o.base.flags |= GUI_FLAG_DIRTY

struct gui_button_t;
struct gui_select_t;
struct gui_select_option_t;

typedef void (*gui_button_pressed_f)(void);
typedef void (*gui_select_selected)(struct gui_select_option_t* selected);

/*
 * Take care when you change any of this. Defines below use memory shenanigans as optimization.
 */

struct gui_edit_t
{
    struct gui_object_t base;
    uint8_t* cursor_pixels_addr;
    uint8_t* cursor_color_addr;
    uint8_t cursor_even;
    uint16_t value_size;
    char* value;
};

extern void _edit_render();
extern uint8_t _edit_event(enum gui_event_type event_type, void* event);

#define zxgui_edit_init(next, x, y, w, h, buffer, buffer_size) \
    { zxgui_base_init(next, x, y, w, h, _edit_render, _edit_event, GUI_FLAG_DIRTY), NULL, NULL, 0, buffer_size, buffer }

#define zxgui_multiline_edit_init(next, x, y, w, h, buffer, buffer_size) \
    { zxgui_base_init(next, x, y, w, h, _edit_render, _edit_event, GUI_FLAG_DIRTY | GUI_FLAG_MULTILINE), NULL, NULL, 0, buffer_size, buffer }

struct gui_select_t;
typedef uint8_t* (*gui_select_obtain_data_f)(void);

struct gui_select_option_t
{
    uint8_t icon[8];
    uint8_t icon_color;
    void* user;
    char value[];
};

struct gui_select_t
{
    struct gui_object_t base;
    gui_select_obtain_data_f obtain_data_cb;
    void* user;
    uint16_t buffer_offset;
    gui_select_selected selected;
    uint8_t selection;
    uint8_t last_selection;
    uint8_t options_capacity;
    uint8_t options_size;
};

extern void _select_render();
extern uint8_t _select_event(enum gui_event_type event_type, void* event);

#define zxgui_select_init(next, x, y, w, h, obtain_data_cb, options_capacity, user, selected_cb) \
    { zxgui_base_init(next, x, y, w, h, _select_render, _select_event, GUI_FLAG_DIRTY), obtain_data_cb, user, \
      options_capacity * sizeof(struct gui_select_option_t*), selected_cb, 0, 0, options_capacity, 0}

struct gui_image_t
{
    struct gui_object_t base;
    const uint8_t* source;
    const uint8_t* colors;
};

extern void _image_render();

#define zxgui_image_init(next, x, y, w, h, source, colors) \
    { zxgui_base_init(next, x, y, w, h, _image_render, NULL, GUI_FLAG_DIRTY), source, colors}

struct gui_button_t
{
    struct gui_object_t base;
    uint8_t icon;
    const char* title;
    uint8_t key;
    gui_button_pressed_f pressed;
};

extern void _button_render();
extern uint8_t _button_event(enum gui_event_type event_type, void* event);

#define zxgui_button_init(next, x, y, w, h, key, icon, title, pressed) \
    { zxgui_base_init(next, x, y, w, h, _button_render, _button_event, GUI_FLAG_DIRTY), icon, title, key, pressed }

struct gui_label_t
{
    struct gui_object_t base;
    const char* title;
    uint8_t color;
};

extern void _label_render();

#define zxgui_label_init(next, x, y, w, h, title, color, flags) \
    { zxgui_base_init(next, x, y, w, h, _label_render, NULL, GUI_FLAG_DIRTY | (flags)), title, color}

struct gui_scene_t
{
    struct gui_object_t* child;
    struct gui_object_t* focus;
};

extern struct gui_scene_t* current_scene;
extern uint8_t screen_color;
extern uint8_t screen_border;

extern void zxgui_clear(void);
extern void zxgui_scene_init(struct gui_scene_t* scene) __z88dk_fastcall;
extern void zxgui_scene_set(struct gui_scene_t* scene) __z88dk_fastcall;
extern void zxgui_scene_add(struct gui_scene_t* scene, void* object) __z88dk_callee;
extern void zxgui_scene_set_focus(struct gui_scene_t* scene, void* object) __z88dk_callee;
extern void zxgui_scene_dispatch_event(enum gui_event_type event_type, void* event) __z88dk_callee;
extern struct gui_object_t* zxgui_scene_get_last_object(struct gui_scene_t* scene);

#define object_invalidate(flag) this_flags |= flag
#define object_validate() this_flags = this_flags & GUI_FLAGS_CLEAR_DIRTY
#define is_object_invalidated() (this_flags & (GUI_FLAG_DIRTY | GUI_FLAG_DIRTY_INTERNAL))

extern void zxgui_scene_iteration(void);
extern void zxgui_image(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const uint8_t* source, const uint8_t* colors) __z88dk_callee;

extern uint8_t* zxgui_select_add_option(struct gui_select_t* select, const char* option,
    uint8_t option_len, uint16_t user_data_amount, uint8_t* icon, uint8_t icon_color) ZXGUI_CDECL;
extern void zxgui_select_change_option(struct gui_select_t* select, uint8_t option) ZXGUI_CDECL;

extern uint8_t zxgui_label_text_height(uint8_t w, const char* text, uint16_t len, uint8_t max_height) ZXGUI_CDECL;

#define zxgui_screen_color(color) screen_color = color;
extern void zxgui_screen_put(uint8_t x, uint8_t y, uint8_t ch) __z88dk_callee;
extern void zxgui_screen_clear(uint8_t x, uint8_t y, uint8_t w, uint8_t h) __z88dk_callee;
extern void zxgui_screen_recolor(uint8_t x, uint8_t y, uint8_t w, uint8_t h) __z88dk_callee;

#endif