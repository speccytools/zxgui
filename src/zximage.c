#include <string.h>
#include "zxgui.h"

#define THIS_TYPE struct gui_image_t

void _image_render()
{
    if (is_object_invalidated())
    {
        object_validate();
        zxgui_image(this_basics.x, this_basics.y, this_basics.w, this_basics.h, self()->source, self()->colors);
    }
}
