#include "zxgui.h"

struct gui_scene_t* current_scene = NULL;
struct gui_object_t* this = NULL;
uint8_t this_flags = 0;
struct gui_object_basics_t this_basics = {};

struct gui_object_t* zxgui_scene_get_last_object(struct gui_scene_t* scene)
{
    struct gui_object_t* object = scene->child;
    if (object == NULL)
    {
        return NULL;
    }
    while (object->next)
    {
        object = object->next;
    }
    return object;
}

void zxgui_scene_init(struct gui_scene_t* scene) __z88dk_fastcall
{
    scene->child = NULL;
    scene->focus = NULL;
    scene->on_event = NULL;
}

void zxgui_scene_add(struct gui_scene_t* scene, void* object) __z88dk_callee
{
    struct gui_object_t* last = zxgui_scene_get_last_object(scene);
    struct gui_object_t* o = (struct gui_object_t*)object;
    o->next = NULL;
    if (last == NULL)
    {
        scene->child = o;
    }
    else
    {
        last->next = o;
    }
}

static void scene_render()
{
    struct gui_object_t* child = current_scene->child;
    while (child)
    {
        this = child;
        this_basics = child->basics;
        this_flags = child->flags;
        this->render();
        this->flags = this_flags;
        child = child->next;
    }
}

void zxgui_scene_set(struct gui_scene_t* scene) __z88dk_fastcall
{
    current_scene = scene;

    if (current_scene)
    {
        struct gui_object_t *child = current_scene->child;
        while (child)
        {
            child->flags |= GUI_FLAG_DIRTY;
            child = child->next;
        }
    }
}

void zxgui_scene_set_focus(struct gui_scene_t* scene, void* object) __z88dk_callee
{
    scene->focus = object;
}

void zxgui_scene_dispatch_event(enum gui_event_type event_type, void* event) __z88dk_callee
{
    if (current_scene->on_event && current_scene->on_event(event_type, event))
    {
        return;
    }

    struct gui_object_t* child = current_scene->child;
    while (child)
    {
        if (child->event)
        {
            this = child;
            this_basics = child->basics;
            this_flags = child->flags;
            uint8_t caught = this->event(event_type, event);
            this->flags = this_flags;
            if (caught)
            {
                return;
            }
        }
        child = child->next;
    }
}

extern void update_keyboard();

void zxgui_scene_iteration(void)
{
    if (current_scene)
    {
        scene_render();
        update_keyboard();
    }
}