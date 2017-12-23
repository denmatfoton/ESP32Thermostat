#include "CustomWidgets.h"
#include <stdarg.h>



Input::Input()
{
}



NumberInput::NumberInput(int minVal, int maxVal, const char* format) : Input(),
                        maxVal(maxVal), minVal(minVal), value(minVal), format(format)
{
    vPortCPUInitializeMutex(&mutex);
}



void NumberInput::ProcessEvent(GEvent * pe)
{
    switch (pe->type)
    {
    case GEVENT_GWIN_BUTTON:
        if (((GEventGWinButton*)pe)->gwin == ghUpBtn)
        {
            if (value < maxVal)
            {
                taskENTER_CRITICAL(&mutex);
                value++;
                taskEXIT_CRITICAL(&mutex);
                update();
                updateCallback(this);
            }
        }
        else if (((GEventGWinButton*)pe)->gwin == ghDownBtn)
        {
            if (value > minVal)
            {
                taskENTER_CRITICAL(&mutex);
                value--;
                taskEXIT_CRITICAL(&mutex);
                update();
                updateCallback(this);
            }
        }
        break;

    default:
        break;
    }
}


void NumberInput::init(GWidgetInit* pwi, font_t font, void(*updateCallback)(Input*))
{
    GWidgetInit wi;
    x = pwi->g.x;
    y = pwi->g.y;
    this->updateCallback = updateCallback;
    wi = *pwi;

    snprintf(text, MAX_NUMBER_TEXT_LENGTH, format, maxVal);
    text_width = gdispGetStringWidthCount(text, font, 0);
    snprintf(text, MAX_NUMBER_TEXT_LENGTH, format, minVal);
    text_width = max(text_width, gdispGetStringWidthCount(text, font, 0));


    wi.text = NULL;
    wi.g.y += NUMBER_TEXT_Y_SHIFT;
    wi.g.width = text_width;
    wi.g.height = NUMBER_TEXT_HEIGHT;
    label = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(label, text_width, text);
    gwinSetFont(label, font);

    // Apply the button parameters
    wi.g.width = NUMBER_BUTTON_WIDTH;
    wi.g.height = NUMBER_BUTTON_HEIGHT;
    wi.g.x += text_width + NUMBER_BUTTONS_X_SPACE;
    wi.g.y = y;
    wi.text = NULL;
    wi.customDraw = gwinButtonDraw_ArrowUp;
    ghUpBtn = gwinButtonCreate(0, &wi);

    wi.g.y += NUMBER_BUTTON_HEIGHT + NUMBER_BUTTONS_SPACE;
    wi.customDraw = gwinButtonDraw_ArrowDown;
    ghDownBtn = gwinButtonCreate(0, &wi);

    update();
}


void NumberInput::setValue(int value)
{
    if (value >= minVal && value <= maxVal)
    {
        taskENTER_CRITICAL(&mutex);
        this->value = value;
        taskEXIT_CRITICAL(&mutex);
        update();
    }
    else
    {
        updateCallback(this);
    }
}


void NumberInput::update()
{
    taskENTER_CRITICAL(&mutex);
    snprintf(text, MAX_NUMBER_TEXT_LENGTH, format, value);
    taskEXIT_CRITICAL(&mutex);
    gwinRedraw(label);
}


coord_t NumberInput::getWidth()
{
    return text_width + NUMBER_BUTTON_WIDTH + NUMBER_BUTTONS_X_SPACE * 2;
}




Switch::Switch(char* textON, char* textOFF) : Input(), textON(textON), textOFF(textOFF), value(false)
{
    //memcpy(&style, gwinGetDefaultStyle(), sizeof(style));
    //memcpy(&style, &WhiteWidgetStyle, sizeof(style));
}

void Switch::ProcessEvent(GEvent * pe)
{
    switch (pe->type)
    {
    case GEVENT_GWIN_BUTTON:
        if (((GEventGWinButton*)pe)->gwin == button)
        {
            value = !value;
            update();
            updateCallback(this);
        }
        break;

    default:
        break;
    }
}


void Switch::init(GWidgetInit * pwi, font_t font, void(*updateCallback)(Input *))
{
    GWidgetInit wi;
    this->updateCallback = updateCallback;
    wi = *pwi;

    wi.text = NULL;
    wi.customDraw = gwinButtonDraw_Rounded;
    button = gwinButtonCreate(0, &wi);
    gwinSetFont(button, font);
    gwinSetStyle(button, &style);

    update();
}


void Switch::setValue(bool value)
{
    this->value = value;
    update();
}


void Switch::update()
{
    memcpy(&style, gwinGetDefaultStyle(), sizeof(style));

    if (value == true)
    {
        gwinSetText(button, textON, FALSE);
        style.enabled.fill = Green;
        gwinRedraw(button);
    }
    else
    {
        gwinSetText(button, textOFF, FALSE);
        style.enabled.fill = Red;
        gwinRedraw(button);
    }
}
