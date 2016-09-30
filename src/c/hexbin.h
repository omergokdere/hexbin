#pragma once

#include <pebble.h>

/* number of hex digits that fit on a line */
#define MAX_PROMPT_LEN 6

/* number of binary digits to display at a time */
#define MAX_GUESS_LINE 4

/* sleep times for various events, in milliseconds */
extern const int correct_time;
extern const int incorrect_time;
extern const int report_time;
extern const int long_click_time;

/* the upper line displays random hex digit/s */
extern uint64_t prompt;
extern int prompt_len;
extern char prompt_text[MAX_PROMPT_LEN + 1];

/* the user keys the binary form into the lower line */
extern uint64_t guess;
extern int guess_len;
extern char guess_text[MAX_GUESS_LINE + 1];

extern TextLayer *prompt_display, *guess_display;

void callback(void *fn);
void check_correct(void);
void input_bit(bool bit);
void new_prompt(void);
void redraw(void);