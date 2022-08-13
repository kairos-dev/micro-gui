#include <stdint.h>

#include "ugui/ugui.h"
#include "ugui/font.h"

#include "ugui/widgets/text_widget.h"

#include "watchface.h"

static ugui_window_t *text_window;
static ugui_text_widget_t* time_widget;
static ugui_text_widget_t* date_widget;

static const char* time = "HH:mm";
static const char* date = "DD/MM/YYYY";

ugui_window_t* watchface_create(uint32_t w, uint32_t h)
{
    text_window = ugui_window_create(w, h);

    time_widget = ugui_text_widget_create((ugui_rect_t) {.x = 50, .y = 50, .w = 200, .h = 50});
    date_widget = ugui_text_widget_create((ugui_rect_t) {.x = 5, .y = 90, .w = 300, .h = 100 });

    ugui_text_widget_set_text(time_widget, &font_robotomono_regular_32, time, 0);
    ugui_text_widget_set_text(date_widget, &font_robotomono_regular_32, date, 0);

    ugui_layer_t* time_layer = ugui_window_get_base_layer(text_window);
    ugui_layer_t* date_layer = ugui_window_get_base_layer(text_window);

    ugui_layer_t* time_widget_layer = ugui_text_widget_get_layer(time_widget);
    ugui_layer_t* date_widget_layer = ugui_text_widget_get_layer(date_widget);

    ugui_layer_add_child(time_layer, time_widget_layer);
    ugui_layer_add_child(date_layer, date_widget_layer);

    return text_window;
}

void watchface_destroy(){
    ugui_text_widget_destroy(time_widget);

    ugui_window_destroy(text_window);
}
