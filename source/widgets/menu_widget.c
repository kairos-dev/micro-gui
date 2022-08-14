
#include "ugui/widgets/menu_widget.h"

#include <stdio.h>
#include <stdlib.h>

#define UGUI_MENU_WIDGET_MAX_CELLS	8
//TODO: assert to ensure num cells < num layers

struct ugui_menu_widget_s {
	ugui_layer_t* widget_layer;
	ugui_menu_widget_draw_callbacks_t draw_callbacks;
	ugui_menu_widget_data_callbacks_t data_callbacks;

	uint32_t selected;
	uint32_t offset;
	void* data;
};

static void menu_widget_layer_update(ugui_layer_t* layer, void* graphics_ctx, void* layer_ctx)
{
	ugui_menu_widget_t* menu_widget = (ugui_menu_widget_t*) layer_ctx;
	ugui_rect_t* bounds = ugui_layer_get_bounds(layer);

	int32_t num_rows = menu_widget->data_callbacks.get_num_rows(menu_widget, menu_widget->data);
	int32_t header_height = menu_widget->draw_callbacks.get_header_height(menu_widget, menu_widget->data);
	int32_t row_height = menu_widget->draw_callbacks.get_cell_height(menu_widget, menu_widget->data);

	int32_t max_rows = (bounds->h - header_height) / row_height;
	int32_t min_rows = (num_rows < max_rows) ? num_rows : max_rows;

	ugui_rect_t cell_bounds;
	cell_bounds.x = 0;
	cell_bounds.y = header_height;
	cell_bounds.w = bounds->w;
	cell_bounds.h = row_height;

	char header[64];
	char title[64];
	char data[64];

	//Draw header
	menu_widget->data_callbacks.get_header(menu_widget, header);
	menu_widget->draw_callbacks.draw_header(graphics_ctx, bounds, header);

	//Draw cells
	//TODO: support for 2d cells
	for (uint8_t i = 0; i <= min_rows; i++) {
		//Calculate row offset in pixels
		cell_bounds.y = header_height + (i-menu_widget->selected) * row_height;

		menu_widget->data_callbacks.get_data(menu_widget, i + menu_widget->offset, title, data);
		menu_widget->draw_callbacks.draw_row(graphics_ctx, &cell_bounds, title, data);

		//Highlight selected cell
		if ((i + menu_widget->offset) == menu_widget->selected) {
			ugui_graphics_inverse_rect(graphics_ctx,
			(ugui_point_t) {.x = cell_bounds.x, .y = cell_bounds.y},
			(ugui_size_t) {.w = cell_bounds.w, .h = cell_bounds.h});
		}
	}
}

static uint32_t ugui_menu_widget_basic_get_header_height(void *menu_layer, void* data)
{
	return font_robotomono_regular_18.glyph_height + 4; //Padding, todo: config struct
}

static void ugui_menu_widget_basic_header_draw(ugui_graphics_t *graphics_ctx, ugui_rect_t* bounds, char* title)
{
	int w = bounds->w;
	int h = bounds->h;

	ugui_size_t text_size;
	ugui_font_get_text_size(&font_robotomono_regular_18,
	                        title,
	                        &text_size);

	ugui_graphics_draw_text(graphics_ctx,
	                        title,
	                        &font_robotomono_regular_18,
	(ugui_point_t) {
		.x = w / 2 - text_size.w / 2, .y = 0
	});
}

static uint32_t ugui_menu_widget_basic_get_cell_height(void *menu_layer, void* data)
{
	return font_robotomono_regular_18.glyph_height + font_robotomono_regular_16.glyph_height + 16;
}

static void ugui_menu_widget_basic_cell_draw(ugui_graphics_t* graphics_ctx, ugui_rect_t* bounds, char* title , char* data)
{
	int w = bounds->w;
	int h = bounds->h;
	int x = bounds->x;
	int y = bounds->y;

	//Surround
	ugui_graphics_draw_rect(graphics_ctx,
	(ugui_point_t) {.x = x, .y = y},
	(ugui_size_t) {.w = w - 1, .h = h});
#if 1
	//Header
	ugui_size_t title_size;
	ugui_font_get_text_size(&font_robotomono_regular_18,
	                        title,
	                        &title_size);

	ugui_graphics_draw_text(graphics_ctx,
	                        title,
	                        &font_robotomono_regular_18,
	(ugui_point_t) {
		.x = x + w / 2 - title_size.w / 2, .y = y
	});

	//Data
	ugui_size_t data_size;
	ugui_font_get_text_size(&font_robotomono_regular_16,
	                        data,
	                        &data_size);

	ugui_graphics_draw_text(graphics_ctx,
	                        data,
	                        &font_robotomono_regular_16,
	(ugui_point_t) {
		.x = x + w / 2 - data_size.w / 2, .y = y + title_size.h + 8
	});
#endif
}

static void ugui_menu_widget_basic_event_handler(ugui_window_t *window, int event, void* ctx) {
	ugui_menu_widget_t* menu_widget = (ugui_menu_widget_t*)ctx;

	int max_rows = menu_widget->data_callbacks.get_num_rows(menu_widget, NULL);

	switch (event) {
        case UGUI_EVT_UP:
            if (menu_widget->selected > 0)
                {
                    menu_widget->selected --;
                    if(menu_widget->data_callbacks.index != NULL) {
                        menu_widget->data_callbacks.index(menu_widget, menu_widget->selected, NULL);
                    }
                }
            break;
        case UGUI_EVT_DOWN:
            if (menu_widget->selected < (max_rows - 1))
            {
                menu_widget->selected ++;
                if(menu_widget->data_callbacks.index != NULL) {
                    menu_widget->data_callbacks.index(menu_widget, menu_widget->selected, NULL);
                }
            }
            break;
        case UGUI_EVT_RIGHT:
        case UGUI_EVT_SELECT:
            //TODO: select callback
            if(menu_widget->data_callbacks.select != NULL) {
                menu_widget->data_callbacks.select(menu_widget, menu_widget->selected, NULL);
            }
            break;
        case UGUI_EVT_BACK:
        case UGUI_EVT_LEFT:
            if(menu_widget->data_callbacks.exit != NULL) {
                menu_widget->data_callbacks.exit(menu_widget, menu_widget->selected, NULL);
            }
            break;
        default:
            break;
	}
}

ugui_menu_widget_t *ugui_menu_widget_create(ugui_rect_t bounds) {
	ugui_menu_widget_t* menu_widget = (ugui_menu_widget_t*)malloc(sizeof(ugui_menu_widget_t));

	menu_widget->widget_layer = ugui_layer_create(bounds);

	//Set location information
	menu_widget->selected = 0;
	menu_widget->offset = 0;

	//Default draw callbacks
	menu_widget->draw_callbacks.draw_header = ugui_menu_widget_basic_header_draw;
	menu_widget->draw_callbacks.draw_row = ugui_menu_widget_basic_cell_draw;
	menu_widget->draw_callbacks.get_header_height = ugui_menu_widget_basic_get_header_height;
	menu_widget->draw_callbacks.get_cell_height = ugui_menu_widget_basic_get_cell_height;

	//Bind update method
	ugui_layer_set_update(menu_widget->widget_layer, menu_widget_layer_update);
	//Bind menu_widget instance to layer context for use in update and other calls
	ugui_layer_set_ctx(menu_widget->widget_layer, (void*) menu_widget);

	return menu_widget;
}

void ugui_menu_widget_destroy(ugui_menu_widget_t* menu_widget)
{
	ugui_layer_destroy(menu_widget->widget_layer);

	free(menu_widget);
}

void ugui_menu_widget_set_callbacks(ugui_menu_widget_t* menu_widget, ugui_menu_widget_data_callbacks_t* callbacks)
{
	menu_widget->data_callbacks.get_num_sections = callbacks->get_num_sections;
	menu_widget->data_callbacks.get_num_rows = callbacks->get_num_rows;
	menu_widget->data_callbacks.get_header = callbacks->get_header;
	menu_widget->data_callbacks.get_data = callbacks->get_data;
	menu_widget->data_callbacks.select = callbacks->select;
	menu_widget->data_callbacks.index = callbacks->index;
	menu_widget->data_callbacks.exit = callbacks->exit;
}

void ugui_menu_widget_set_draw(ugui_menu_widget_t* menu_widget, ugui_menu_widget_draw_callbacks_t* callbacks)
{
	menu_widget->draw_callbacks.draw_header = callbacks->draw_header;
	menu_widget->draw_callbacks.draw_row = callbacks->draw_row;
	menu_widget->draw_callbacks.get_header_height = callbacks->get_header_height;
	menu_widget->draw_callbacks.get_cell_height = callbacks->get_cell_height;
}

void ugui_menu_widget_attach_to_window(ugui_menu_widget_t* menu_widget, ugui_window_t* window)
{
	//TODO: attach handlers for scroll and select (and back?)
	ugui_window_set_event_handler(window, ugui_menu_widget_basic_event_handler, (void*) menu_widget);
}

ugui_layer_t* ugui_menu_widget_get_layer(ugui_menu_widget_t* menu_widget)
{
	return menu_widget->widget_layer;
}
