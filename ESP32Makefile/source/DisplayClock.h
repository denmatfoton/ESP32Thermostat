#pragma once
#include <Arduino.h>
#include "gfx.h"
#include "RtcClock.h"



enum ClockStyle
{
    DISPLAY_SECONDS,
    DISPLAY_ONLY_MINUTES
};


#define MAX_TIME_TEXT_LENGTH   10
#define TEXT_SECONDS_OFFSET    7
#define GEVENT_CLOCK           GEVENT_USER_FIRST
#define GCLOCK_SOURCE          ((GSourceHandle)(void *)&rtc_time)

class DisplayClock
{
public:
    DisplayClock(ClockStyle style = DISPLAY_ONLY_MINUTES);
    void init(GWidgetInit* pwi, font_t font, coord_t height);
    void ProcessEvent(GEvent* pe);
    coord_t getWidth();
private:
    void update();
    char text[MAX_TIME_TEXT_LENGTH];
    char* main_format;
    bool main_part_actual;
    const ClockStyle style;
    font_t font;
    coord_t x, y;
    coord_t main_width;
    coord_t seconds_width;
    GHandle main_label;
    GHandle seconds_label;
};


typedef struct GEventClock {
    GEventType		type;		// The type of this event (GEVENT_CLOCK)
} GEventClock;
