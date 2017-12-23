#include <Arduino.h>
#include <SPI.h>
#include "cfg.h"
#include "DisplayHandler.h"
#include "DisplayUtils.h"
#include "CustomWidgets.h"


#define DISPLAY_TASK_STACK_SIZE   4096//2048
#define DISPLAY_TASK_PIORITY      1

#define	ScrWidth			gdispGetWidth()
#define	ScrHeight			gdispGetHeight()

TaskHandle_t displayTaskHandle;
ThermostatWidget thermostatWi;

static void DisplayTask(void* param);


void DisplaySetup()
{
    // Initialize the display
    gfxInit();

    xTaskCreatePinnedToCore(DisplayTask, "DisplayTask", DISPLAY_TASK_STACK_SIZE, NULL, DISPLAY_TASK_PIORITY, &displayTaskHandle, APP_CPU_NUM);
}



void inputCallback(Input* input)
{
    if (input == &thermostatWi.maxTempInput)
    {
        thermoController.setMaxTemperature(thermostatWi.maxTempInput.getValue());
    }
    else if (input == &thermostatWi.minTempInput)
    {
        thermoController.setMinTemperature(thermostatWi.minTempInput.getValue());
    }
    else if (input == &thermostatWi.thermostatEnabledInput)
    {
        thermoController.setEnabled(thermostatWi.thermostatEnabledInput.getValue());
    }
    else if (input == &thermostatWi.thermostatModeInput)
    {
        thermoController.setMode(thermostatWi.thermostatModeInput.getValue());
        thermostatWi.updateStyle();
    }
    else if (input == &thermostatWi.fanEnabledInput)
    {
        Serial.println("Fan Enabled Callback");
        thermoController.setFanEnabled(thermostatWi.fanEnabledInput.getValue());
    }
}


ThermostatWidget::ThermostatWidget() : maxTempInput(15, 50, "Max:%dC"),
                                       minTempInput(15, 50, "Min:%dC"),
                                       thermostatEnabledInput("ON", "OFF"),
                                       fanEnabledInput("ON", "OFF"),
                                       thermostatModeInput()
{
}


#define ENABLE_BUTTON_WIDTH       70
#define ENABLE_BUTTON_HEIGHT      45
#define MODE_RADIO_HEIGHT         30
#define MODE_RADIO_WIDTH          120
#define ELEMENTS_SPACE            3
#define TEMP_LABEL_HEIGHT         20
#define TEMP_LABEL_TAB            100
#define TEMP_LABEL_WIDTH          (TEMP_LABEL_TAB + 80)

#define FAN_BUTTON_WIDTH          60
#define FAN_BUTTON_HEIGHT         35

void ThermostatWidget::createThermostatWidget(void)
{
    GWidgetInit wi;

    // Apply some default values for GWIN
    gwinWidgetClearInit(&wi);

    // Apply the container parameters
    wi.g.show = FALSE;
    wi.g.width = ScrWidth;
    wi.g.height = ScrHeight;
    wi.g.x = 0;
    wi.g.y = 0;
    ghContainer = gwinContainerCreate(0, &wi, 0);

    font_t dejaVuSans20 = gdispOpenFont("DejaVuSans20_aa");
    font_t dejaVuSans16 = gdispOpenFont("DejaVuSans16_aa");
    font_t dejaVuSans12 = gdispOpenFont("DejaVuSans12_aa");

    gwinWidgetClearInit(&wi);
    wi.g.parent = ghContainer;
    wi.g.x = ELEMENTS_SPACE;
    wi.g.y = ELEMENTS_SPACE;
    wi.g.show = TRUE;
    minTempInput.init(&wi, dejaVuSans20, inputCallback);
    
    wi.g.x += minTempInput.getWidth() + 10;
    maxTempInput.init(&wi, dejaVuSans20, inputCallback);

    wi.g.x = ELEMENTS_SPACE + (MODE_RADIO_WIDTH - ENABLE_BUTTON_WIDTH) / 2;
    wi.g.y += NUMBER_INPUT_HEIGHT + 10;
    wi.g.width = ENABLE_BUTTON_WIDTH;
    wi.g.height = ENABLE_BUTTON_HEIGHT;
    thermostatEnabledInput.init(&wi, dejaVuSans20, inputCallback);

    wi.g.x = ELEMENTS_SPACE;
    wi.g.y += ENABLE_BUTTON_HEIGHT + 10;
    wi.g.width = MODE_RADIO_WIDTH;
    wi.g.height = MODE_RADIO_HEIGHT;
    thermostatModeInput.init(&wi, dejaVuSans12, inputCallback, "Normal mode", "Night mode", "Bedroom off");

    createTempLabels(wi);


    wi.g.parent = ghContainer;
    wi.g.x = ELEMENTS_SPACE + MODE_RADIO_WIDTH + 90;
    wi.g.y += TEMP_LABEL_HEIGHT + 10 + NUMBER_INPUT_HEIGHT + 15;
    wi.g.width = 40;
    GHandle nameLabel = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(nameLabel, 40, "Fan");
    gwinSetFont(nameLabel, dejaVuSans16);

    wi.g.x += 40;
    wi.g.y -= 5;
    wi.g.width = FAN_BUTTON_WIDTH;
    wi.g.height = FAN_BUTTON_HEIGHT;
    fanEnabledInput.init(&wi, dejaVuSans20, inputCallback);

    minTempInput.setValue(thermoController.getMinTemperature());
    maxTempInput.setValue(thermoController.getMaxTemperature());
    thermostatEnabledInput.setValue(thermoController.getEnabled());
    thermostatModeInput.setValue(thermoController.getThermoMode());
    fanEnabledInput.setValue(false);

    // Make the container become visible - therefore all its children
    // become visible as well
    gwinShow(ghContainer);
}


inline void ThermostatWidget::createTempLabels(GWidgetInit & wi)
{
    wi.g.x = ELEMENTS_SPACE + MODE_RADIO_WIDTH + 10;
    wi.g.y = NUMBER_INPUT_HEIGHT + 10;
    wi.g.width = ScrWidth - wi.g.x - ELEMENTS_SPACE;
    wi.g.height = (TEMPR_COUNT + 1) * (TEMP_LABEL_HEIGHT + ELEMENTS_SPACE) + 1;//ScrHeight - wi.g.y - ELEMENTS_SPACE;
    ghTemperatureContainer = gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);

    font_t dejaVuSans16 = gdispOpenFont("DejaVuSans16_aa");
    font_t dejaVuSans12 = gdispOpenFont("DejaVuSans12_aa");

    gwinWidgetClearInit(&wi);
    wi.g.parent = ghTemperatureContainer;
    wi.g.x = ELEMENTS_SPACE;
    wi.g.y = ELEMENTS_SPACE;
    wi.g.width = TEMP_LABEL_WIDTH;
    wi.g.height = TEMP_LABEL_HEIGHT;
    wi.g.show = TRUE;

    wi.g.x = 45;
    GHandle nameLabel = gwinLabelCreate(0, &wi);
    gwinLabelSetAttribute(nameLabel, TEMP_LABEL_TAB, "Sensor data");
    gwinSetFont(nameLabel, dejaVuSans16);

    wi.g.x = ELEMENTS_SPACE;
    for (int i = 0; i < TEMPR_COUNT; ++i)
    {
        wi.g.y += TEMP_LABEL_HEIGHT + ELEMENTS_SPACE;
        ghTempLabels[i] = gwinLabelCreate(0, &wi);
        gwinLabelSetAttribute(ghTempLabels[i], TEMP_LABEL_TAB, getTemperatureName(i));
        gwinSetFont(ghTempLabels[i], dejaVuSans12);
    }
}


void ThermostatWidget::updateTemperatureAndHumidity(const uint8_t idx, const float temp, const int hum)
{
    static char text_temp[20];
    if (idx >= TEMPR_COUNT)
    {
        return;
    }
    if (hum >= 0)
    {
        snprintf(text_temp, sizeof(text_temp), "%.1fC, %d%%", temp, hum);
    }
    else
    {
        snprintf(text_temp, sizeof(text_temp), "%.1fC", temp);
    }

    gwinSetText(ghTempLabels[idx], text_temp, TRUE);
}


void ThermostatWidget::updateStyle()
{
    if (thermostatModeInput.getValue() == NIGHT_MODE)
    {
        if (gwinGetDefaultStyle() != &BlackWidgetStyle)
        {
            gwinSetDefaultStyle(&BlackWidgetStyle, TRUE);
            thermostatEnabledInput.update();
            fanEnabledInput.update();
        }
    }
    else if (thermostatModeInput.getValue() == NORMAL_MODE ||
            thermostatModeInput.getValue() == BEDROOM_CLOSE_MODE)
    {
        if (gwinGetDefaultStyle() != &WhiteWidgetStyle)
        {
            gwinSetDefaultStyle(&WhiteWidgetStyle, TRUE);
            thermostatEnabledInput.update();
            fanEnabledInput.update();
        }
    }
}


void ThermostatWidget::processEvent(GEvent* pe)
{
    switch (pe->type)
    {
    case GEVENT_GWIN_BUTTON:
        maxTempInput.ProcessEvent(pe);
        minTempInput.ProcessEvent(pe);
        thermostatEnabledInput.ProcessEvent(pe);
        fanEnabledInput.ProcessEvent(pe);
        break;

    case GEVENT_GWIN_RADIO:
        thermostatModeInput.ProcessEvent(pe);
        break;
    default:
        break;
    }
}



static void DisplayTask(void* param)
{
    static GEvent* pe;
    static GListener	gl;

    // Set the widget defaults
    gwinSetDefaultFont(gdispOpenFont("*"));
    gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
    gdispClear(White);

    // Create the widget
    thermostatWi.createThermostatWidget();

    geventListenerInit(&gl);
    gwinAttachListener(&gl);

    while (1)
    {
        // Get an Event
        pe = geventEventWait(&gl, TIME_INFINITE);

        thermostatWi.processEvent(pe);
    }
}
