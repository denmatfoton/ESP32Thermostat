#pragma once
#include <Arduino.h>
#include "CmdHandler.h"
#include "gfx.h"


#define NUMBER_BUTTON_HEIGHT      30
#define NUMBER_BUTTON_WIDTH       50
#define NUMBER_BUTTONS_SPACE      3
#define NUMBER_INPUT_HEIGHT       (2 * NUMBER_BUTTON_HEIGHT + NUMBER_BUTTONS_SPACE)

#define NUMBER_TEXT_Y_SHIFT       17
#define NUMBER_TEXT_HEIGHT        30

#define NUMBER_BUTTONS_X_SPACE    3

#define MAX_NUMBER_TEXT_LENGTH    20


#define RADIOS_GROUP_START      0


class Input
{
public:
    Input();
    virtual void ProcessEvent(GEvent* pe) = 0;
protected:
    void(*updateCallback) (Input*);
};


class NumberInput : public Input
{
public:
    NumberInput(int minVal, int maxVal, const char* format);
    void init(GWidgetInit* pwi, font_t font, void(*updateCallback) (Input*));
    void setValue(int value);
    void ProcessEvent(GEvent* pe) override;
    int getValue() { return value; }
    coord_t getWidth();
private:
    void update();
    int value;
    int maxVal, minVal;
    char text[MAX_NUMBER_TEXT_LENGTH];
    const char* format;

    portMUX_TYPE mutex;
    coord_t text_width;
    coord_t x;
    coord_t y;
    GHandle label;
    GHandle ghUpBtn;
    GHandle ghDownBtn;
};


class Switch : public Input
{
public:
    Switch(char* textON, char* textOFF);
    void ProcessEvent(GEvent* pe) override;
    void init(GWidgetInit* pwi, font_t font, void(*updateCallback) (Input*));
    void setValue(bool value);
    bool getValue() { return value; }
    void update();
private:
    bool value;

    char* textON;
    char* textOFF;
    GWidgetStyle style;
    GHandle button;
};




template <class T, int size>
class Radio : public Input
{
public:
    Radio();
    void ProcessEvent(GEvent* pe) override;
    void init(GWidgetInit* pwi, font_t font, void(*updateCallback) (Input*), ...);
    void setValue(T value);
    T getValue() { return value; }
private:
    T value;

    static int groupsCount;
    const int currentGroup;
    GHandle radios[size];
};



template<class T, int size>
int Radio<T, size>::groupsCount = RADIOS_GROUP_START;


template<class T, int size>
Radio<T, size>::Radio() : Input(), currentGroup(groupsCount), value(static_cast<T>(0))
{
    groupsCount++;
}


template<class T, int size>
void Radio<T, size>::ProcessEvent(GEvent * pe)
{
    switch (pe->type)
    {
    case GEVENT_GWIN_RADIO:

        if (((GEventGWinRadio *)pe)->group == currentGroup)
        {
            for (int i = 0; i < size; ++i)
            {
                if (((GEventGWinRadio *)pe)->gwin == radios[i])
                {
                    value = static_cast<T>(i);
                    break;
                }
            }

            updateCallback(this);
        }
        break;

    default:
        break;
    }
}


template<class T, int size>
void Radio<T, size>::init(GWidgetInit * pwi, font_t font, void(*updateCallback)(Input *), ...)
{
    va_list arguments;
    GWidgetInit wi;
    this->updateCallback = updateCallback;
    wi = *pwi;

    va_start(arguments, updateCallback);
    for (int i = 0; i < size; i++)
    {
        wi.text = va_arg(arguments, char*);
        radios[i] = gwinRadioCreate(0, &wi, currentGroup);
        wi.g.y += wi.g.height + 3;
        if (font != NULL)
        {
            gwinSetFont(radios[i], font);
        }
    }
    va_end(arguments);

    gwinRadioPress(radios[0]);
}


template<class T, int size>
void Radio<T, size>::setValue(T value)
{
    this->value = value;
    gwinRadioPress(radios[static_cast<int>(value)]);
}
