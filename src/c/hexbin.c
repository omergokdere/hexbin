#include <pebble.h>
#include <inttypes.h>
#include "hexbin.h"
#include "util.h"

/* sleep times for various events, in milliseconds */
const int correct_time = 1000;
const int incorrect_time = 2000;
const int report_time = 200;
const int long_click_time = 500;

/* the upper line displays random hex digit/s */
uint64_t prompt;
int prompt_len;
char prompt_text[MAX_PROMPT_LEN + 1];

/* the user keys the binary form into the lower line */
uint64_t guess;
int guess_len;
char guess_text[MAX_GUESS_LINE + 1];

TextLayer *prompt_display, *guess_display;

/* turns a procedure with no arguments into a procedure taking a null pointer */
void callback(void *fn) { ((void (*)(void)) fn)(); }

void new_prompt(void)
{
	/* come up with a new prompt */
	uint64_t last_prompt = prompt;
	uint64_t max_num = (1u << prompt_len * 4) - 1;
	do {
		prompt = rand() & max_num;
	} while (prompt == last_prompt);
}

void input_bit(bool bit)
{
	/* don't accept input while waiting for callbacks */
	if (guess_len >= prompt_len * 4)
		return;
	
	guess <<= 1;
	if (bit)
		guess |= 1;
	guess_len++;
	redraw();
	
	if (guess_len == prompt_len * 4)
		app_timer_register(report_time, callback, check_correct);
}

void check_correct(void)
{
	bool correct = (guess == prompt);
	if (!correct) {
		app_timer_register(0, callback, vibes_double_pulse);
		guess = prompt;
		redraw();
	} else {
		text_layer_set_text(prompt_display, "\\o/");
		new_prompt();
	}
	
	guess = guess_len = 0;
	app_timer_register(correct ? correct_time : incorrect_time, callback, redraw);
}

void redraw(void)
{
	/* display prompt in hex, digit by digit */
	for (int i = 0; i < prompt_len; i++)
		snprintf(prompt_text + i, 2, "%" PRIx64,
			 NIBBLEFIELD_INDEX_LTR(prompt, i, prompt_len));
	text_layer_set_text(prompt_display, prompt_text);
	
	/* if no guess entered, display a question mark as the guess */
	if (!guess_len) {
		text_layer_set_text(guess_display, "?");
		return;
	}
	
	/*
	 * display groups of up to max_guess_line input bits at a time. e.g.
	 * if a user were inputting a pattern of three ones followed by three
	 * zeros repeating, the display would look like this at each press:
	 *
	 *     1 -> 11 -> 111 -> 1110 ->
	 *     0 -> 00 -> 001 -> 0011 ->
	 *     1 -> 10 -> 100 -> 1000 -> repeat
	 */
	
	/* the number of items in the line */
	int line_len = (guess_len - 1) % MAX_GUESS_LINE + 1;
	
	/* the line we're on */
	int line_num = (guess_len - 1) / MAX_GUESS_LINE;
	
	/* the first element of the line as a bit offset */
	int bit_offset = line_num * MAX_GUESS_LINE;
	
	/* clear the line */
	guess_text[0] = 0;
	
	bool bit;
	int max = ARRAY_SIZE(guess_text);
	for (int i = 0; i < line_len; i++, bit_offset++, max--) {
		bit = BITFIELD_INDEX_LTR(guess, bit_offset, guess_len);
		snprintf(guess_text + i, max, bit ? "1" : "0");
	}
	
	text_layer_set_text(guess_display, guess_text);
}