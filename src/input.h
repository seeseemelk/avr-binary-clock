#ifndef INPUT_H
#define INPUT_H

#define BUTTON_1 1
#define BUTTON_2 2
#define BUTTON_3 4

#define SWITCH_1 1
#define SWITCH_2 2
#define SWITCH_3 4
#define SWITCH_4 8

#define BUTTONS (BUTTON_1 | BUTTON_2 | BUTTON_3)
#define SWITCHES (SWITCH_1 | SWITCH_2 | SWITCH_3 | SWITCH_4)

#define BUTTON_MUTE BUTTON_1
#define BUTTON_ALARM BUTTON_2
#define BUTTON_TIME BUTTON_3

#define SWITCH_SHOW_SECONDS SWITCH_1
#define SWITCH_SHOW_ALARM SWITCH_2
#define SWITCH_ENABLE_ALARM SWITCH_3
#define SWITCH_CALIBRATE SWITCH_4

void input_init(void);
char input_buttons(void);
char input_debounce_buttons(void);
char input_switches(void);
int input_battery(void);
void input_debounce_tick(void);
void input_debounce_released(void);

#endif
