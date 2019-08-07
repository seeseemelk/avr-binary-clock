#ifndef DISPLAY_H
#define DISPLAY_H

enum DisplayState
{
	DS_HOURS_MINUTES,
	DS_MINUTES_SECONDS,
	DS_CUSTOM
};

void display_init(void);
void display_set(u8 a, u8 b, u8 c, u8 d);
void display_update(void);
void display_draw(void);
void display_set_state(enum DisplayState new_state);

#endif
