#ifndef TIME_H
#define TIME_H

#include "cdefs.h"

void time_init(void);
void time_set(u8 hours, u8 minutes, u8 seconds);
void time_add_seconds(u8 seconds);
void time_get_hours(u8* hour_h, u8* hour_t);
void time_get_minutes(u8* minutes_h, u8* minutes_t);
void time_get_seconds(u8* seconds_h, u8* seconds_t);
void time_set_calibration(u16 calibration);
u16 time_get_calibration(void);
void alarm_test(void);
void alarm_set(u8 hours, u8 minutes);
void alarm_get(u8* hours, u8* minutes);
void alarm_mute(void);

#endif
