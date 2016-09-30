#include <pebble.h>
#include "hexbin.h"
#include "util.h"

static Window *window;

static void one_bit_handler(ClickRecognizerRef _, void *__) { input_bit(true); }
static void zero_bit_handler(ClickRecognizerRef _, void *__) { input_bit(false); }

static void clear_handler(ClickRecognizerRef _, void *__)
{
	guess = guess_len = 0;
	redraw();
}

static void level_increase_handler(ClickRecognizerRef _, void *__)
{
	prompt_len++;
	if (prompt_len > MAX_PROMPT_LEN)
		prompt_len = 1;
	guess = guess_len = 0;
	new_prompt();
	redraw();
}

static void level_decrease_handler(ClickRecognizerRef _, void *__)
{
	prompt_len--;
	if (prompt_len == 0)
		prompt_len = MAX_PROMPT_LEN;
	guess = guess_len = 0;
	new_prompt();
	redraw();
}

static void mode_switch_handler(ClickRecognizerRef _, void *__)
{
	guess = guess_len = 0;
	new_prompt();
	redraw();
}

static void click_config_provider(void *context)
{
	window_single_click_subscribe(BUTTON_ID_UP, one_bit_handler);
	window_single_click_subscribe(BUTTON_ID_BACK, zero_bit_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, zero_bit_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, clear_handler);
	window_long_click_subscribe(BUTTON_ID_UP, long_click_time, level_increase_handler, NULL);
	window_long_click_subscribe(BUTTON_ID_DOWN, long_click_time, level_decrease_handler, NULL);
	window_long_click_subscribe(BUTTON_ID_SELECT, long_click_time, mode_switch_handler, NULL);
}

static void window_load(Window *window)
{
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);

	prompt_display = text_layer_create(GRect(0, bounds.size.h / 2 - 30, bounds.size.w, 30));
	guess_display = text_layer_create(GRect(0, bounds.size.h / 2, bounds.size.w, 30));
	
	TextLayer *text_layers[2] = {prompt_display, guess_display};
	for (int i = 0; i < (int) ARRAY_SIZE(text_layers); i++) {
		TextLayer *tl = text_layers[i];
		text_layer_set_background_color(tl, GColorBlack);
		text_layer_set_font(tl, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
		text_layer_set_text_color(tl, GColorWhite);
		text_layer_set_text_alignment(tl, GTextAlignmentCenter);
		layer_add_child(window_layer, text_layer_get_layer(tl));
	}
}

static void window_unload(Window *window)
{
	text_layer_destroy(prompt_display);
	text_layer_destroy(guess_display);
}

int main(void)
{
	window = window_create();
	window_set_click_config_provider(window, click_config_provider);
	window_set_background_color(window, GColorBlack);
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	window_stack_push(window, true);
	
	guess = guess_len = 0;
	prompt_len = 1;
	new_prompt();
	redraw();
	app_event_loop();
	
	window_destroy(window);
}