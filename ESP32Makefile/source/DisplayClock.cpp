#include "DisplayClock.h"
#include "src/gwin/gwin_class.h"


static void ClockHandler();

DisplayClock::DisplayClock(ClockStyle style) : style(style), seconds_width(0)
{
    if (style == DISPLAY_SECONDS)
    {
        main_format = "%2d:%02d:";
    }
    else
    {
        main_format = "%2d:%02d";
    }
    main_part_actual = false;
}


void DisplayClock::init(GWidgetInit * pwi, font_t font, coord_t height)
{
    GWidgetInit wi = *pwi;
    this->font = font;

    snprintf(text, TEXT_SECONDS_OFFSET, main_format, 22, 22);
    main_width = gdispGetStringWidthCount(text, font, 0);

    x = wi.g.x;
    y = wi.g.y;
    wi.text = text;
    wi.g.width = main_width;
    wi.g.height = height;
    main_label = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(main_label, 0, " ");
    gwinSetFont(main_label, font);

    if (style == DISPLAY_SECONDS)
    {
        char* seconds_text = text + TEXT_SECONDS_OFFSET;
        snprintf(seconds_text, 3, "%02d", 22);
        seconds_width = gdispGetStringWidthCount(seconds_text, font, 0);

        wi.g.x += main_width;
        wi.g.width = seconds_width;
        seconds_label = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(seconds_label, seconds_width, seconds_text);
        gwinSetFont(seconds_label, font);
    }

    rtc_time.setTimeUpdateCallback(ClockHandler);
}


void DisplayClock::ProcessEvent(GEvent * pe)
{
    if (pe->type == GEVENT_CLOCK)
    {
        update();
    }
}


coord_t DisplayClock::getWidth()
{
    return main_width + seconds_width;
}


void DisplayClock::update()
{
    time_t cur_time;

    if (rtc_time.GetCurrentTime(&cur_time))
    {
        struct tm* ptm;

        ptm = gmtime(&cur_time);

        if (ptm->tm_sec == 0 || !main_part_actual)
        {
            coord_t cur_main_width;

            snprintf(text, TEXT_SECONDS_OFFSET, main_format, ptm->tm_hour, ptm->tm_min);
            cur_main_width = gdispGetStringWidthCount(text, font, 0);
            gwinLabelSetAttribute(main_label, main_width - cur_main_width, " ");
            //main_label->x = x + main_width - cur_main_width;
            gwinRedraw(main_label);
            main_part_actual = true;
        }

        if (style == DISPLAY_SECONDS)
        {
            snprintf(text + TEXT_SECONDS_OFFSET, 3, "%02d", ptm->tm_sec);
            gwinRedraw(seconds_label);
        }
    }
}


static void ClockHandler()
{
    static GSourceListener*	psl = 0;
    static GEvent*			eventBuffer;

    while (psl = geventGetSourceListener(GCLOCK_SOURCE, psl))
    {
        if (!(eventBuffer = geventGetEventBuffer(psl)))
            continue;

        eventBuffer->type = GEVENT_CLOCK;

        geventSendEvent(psl);
    }
}
