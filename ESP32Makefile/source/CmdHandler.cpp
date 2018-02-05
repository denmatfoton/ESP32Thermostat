#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CmdHandler.h"
#include "ThermostatController.h"
#include "DisplayHandler.h"


CmdManager_c CmdManager;


const char* temperatureIdxToString[] = { "Bedroom", "Living room", "Center", "Outside" };


#define setValue(func)        thermoController.func; thermostatWi.func;


void updateSensorDataByIdx(TemperatureIdx idx, const char* payload)
{
    char* pos = strstr(payload, "temp");
    float temperatute = 0;
    int humidity = -1;

    if (pos != nullptr)
    {
        pos = strchr(pos, ':');
        if (pos != nullptr) temperatute = atof(pos + 1);
        thermoController.updateTemperature(idx, temperatute);
    }
    pos = strstr(payload, "hum");
    if (pos != nullptr)
    {
        pos = strchr(pos, ':');
        if (pos != nullptr) humidity = atoi(pos + 1);
    }
    thermostatWi.updateTemperatureAndHumidity(idx, temperatute, humidity);
}


void CmdManager_c::ProcessMqttPayload(const char* topic, const char* payload) const
{
    if (strcmp(topic, POWER_TOPIC) == 0)
    {
        if (strcmp(payload, "on") == 0)
        {
            setValue(setEnabled(true));
        }
        else if (strcmp(payload, "off") == 0)
        {
            setValue(setEnabled(false));
        }
    }
    else if (strcmp(topic, FAN_TOPIC) == 0)
    {
        if (strcmp(payload, "enabled") == 0)
        {
            setValue(setFanEnabled(true));
        }
        else if (strcmp(payload, "disabled") == 0)
        {
            setValue(setFanEnabled(false));
        }
    }
    else if (strcmp(topic, TARGET_TEMP_TOPIC) == 0)
    {
        if (payload[0] >= '0' && payload[0] <= '9')
        {
            const float value = atof(payload);
            setValue(setTargetTemperature(value));
        }
    }
    else if (strcmp(topic, MODE_TOPIC) == 0)
    {
        if (strcmp(payload, "night") == 0)
        {
            setValue(setMode(NIGHT_MODE));
        }
        else if (strcmp(payload, "normal") == 0)
        {
            setValue(setMode(NORMAL_MODE));
        }
        else if (strcmp(payload, "bedroom_off") == 0)
        {
            setValue(setMode(BEDROOM_OFF_MODE));
        }
    }
    else if (strcmp(topic, BEDROOM_TEMP_TOPIC) == 0)
    {
        updateSensorDataByIdx(BEDROOM_IDX, payload);
    }
    else if (strcmp(topic, LIVINGROOM_TEMP_TOPIC) == 0)
    {
        updateSensorDataByIdx(LIVINGROOM_IDX, payload);
    }
    else if (strcmp(topic, OUTSIDE_TEMP_TOPIC) == 0)
    {
        updateSensorDataByIdx(OUTSIDE_IDX, payload);
    }
}

