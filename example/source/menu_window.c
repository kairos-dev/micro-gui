
#include "menu_window.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ugui/ugui.h"
#include "ugui/font.h"

#include "ugui/widgets/menu_widget.h"

#include "line_window.h"
#include "circle_window.h"
#include "layer_window.h"
#include "text_window.h"

//Demo type for use in menu widget
typedef struct demo_s {
	char* name;
	char* desc;
	ugui_window_t** window;
} demo_t;

//External (static) gui context
//TODO: could make this prettier
extern ugui_t* gui;

//Parent (this) window and widget
ugui_window_t *menu_window;
ugui_menu_widget_t *menu_widget;

//Child windows
ugui_window_t* line_test_window;
ugui_window_t* circle_test_window;
ugui_window_t* layer_test_window;
ugui_window_t* text_test_window;


//Demo list (for use in menu widget)
demo_t demos[] = {
	{"Calendar", "eventos importantes", &layer_test_window},
	{"Alarmes", "acorde cedo", &circle_test_window},
	{"Timer", "conte o tempo", &line_test_window},
	{"Settings", "configuracoes", &text_test_window}
};

char* title = "22:18 - 10/08/22";

/***			Internal Functions				***/

//TODO: unused
static uint32_t example_menu_get_num_sections(void *menu_layer, void* data)
{
	return 0;
}

//Row count handler for menu widget
//Lets the menu widget know how many rows of data are available
static uint32_t example_menu_get_num_rows(void *menu_layer, void* data)
{
	return sizeof(demos) / sizeof(demo_t);
}

static void example_menu_get_header(void* menu_layer, char* header) 
{
	strncpy(header, title, strlen(title) + 1);
}

//Data handler for demo menu
//Provides data for display in the menu widget
static void example_menu_get_data(void* menu_layer, uint16_t index, char* title, char* data)
{
	//TODO: sort it out, shit is unsafe.
	strncpy(title, demos[index].name, strlen(demos[index].name) + 1);
	strncpy(data, demos[index].desc, strlen(demos[index].desc) + 1);
}

//Select handler for demo menu
//Pushes the selected demo window onto the stack
static void example_menu_select(void *menu_layer, uint16_t index, void *callback_context)
{
	ugui_window_t* new_window = *(demos[index].window);
	ugui_window_stack_push(gui, new_window);
}

static void example_menu_index(void *menu_layer, uint16_t index, void *callback_context)
{
    printf("%s - index: %d\n",__func__, index);
}
//Event handler for demo screens
//This pops the demo window off the stack to return to this menu
static void demo_handle_event(ugui_window_t* window, int event, void* ctx)
{
	if ((event == UGUI_EVT_BACK) || event == UGUI_EVT_LEFT) {
		ugui_window_stack_pop(gui);
	}
}

void draw_row_callback(ugui_graphics_t *graphics_ctx, ugui_rect_t *bounds, char* title, char* data)
{
    /* printf("%s - %s\n", __func__, title); */
}

void draw_header_callback(ugui_graphics_t *graphics_ctx, ugui_rect_t *bounds, char* title)
{
    /* printf("%s - %s\n", __func__, title); */
}


uint32_t get_cell_height_callback(void *menu_widget, void* data)
{
    return 100;
}

uint32_t get_header_height_callback(void *menu_widget, void* data)
{
    return 100;
}

/***			External Functions				***/

ugui_window_t *menu_window_create(uint32_t w, uint32_t h)
{
	menu_window = ugui_window_create(w, h);


	//Setup widget
	menu_widget = ugui_menu_widget_create((ugui_rect_t) {
		.w = w,
		.h = h
	});

	ugui_menu_widget_data_callbacks_t example_callbacks;
	example_callbacks.get_num_sections = example_menu_get_num_sections;
	example_callbacks.get_num_rows = example_menu_get_num_rows;
	example_callbacks.get_header = example_menu_get_header;
	example_callbacks.get_data = example_menu_get_data;
	example_callbacks.select = example_menu_select;
	example_callbacks.index = example_menu_index;

	ugui_menu_widget_set_callbacks(menu_widget, &example_callbacks);

    /* ugui_menu_widget_draw_callbacks_t draw_callbacks; */
    /* draw_callbacks.draw_row = draw_row_callback; */
    /* draw_callbacks.draw_header = draw_header_callback; */
    /* draw_callbacks.get_cell_height = get_cell_height_callback; */
    /* draw_callbacks.get_header_height = get_header_height_callback; */

    /* ugui_menu_widget_set_draw(menu_widget, &draw_callbacks); */
	//Bind widget to layer and window context
	ugui_layer_t* base_layer = ugui_window_get_base_layer(menu_window);

	ugui_layer_t* widget_layer = ugui_menu_widget_get_layer(menu_widget);

	ugui_layer_add_child(base_layer, widget_layer);

	ugui_menu_widget_attach_to_window(menu_widget, menu_window);


	//Create child layers
	line_test_window = line_window_create(w, h);
	ugui_window_set_event_handler(line_test_window, demo_handle_event, NULL);

	circle_test_window = circle_window_create(w, h);
	ugui_window_set_event_handler(circle_test_window, demo_handle_event, NULL);

	layer_test_window = layer_window_create(w, h);
	ugui_window_set_event_handler(layer_test_window, demo_handle_event, NULL);

	text_test_window = text_window_create(w, h);
	ugui_window_set_event_handler(text_test_window, demo_handle_event, NULL);

	return menu_window;
}

void menu_window_destroy()
{
	line_window_destroy();
	circle_window_destroy();
	layer_window_destroy();

	ugui_menu_widget_destroy(menu_widget);

	ugui_window_destroy(menu_window);
}


