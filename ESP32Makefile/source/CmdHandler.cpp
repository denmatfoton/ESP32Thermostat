#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "CmdHandler.h"
#include "ThermostatController.h"
#include "DisplayHandler.h"


CmdManager_c CmdManager;


const char cmd_topic[] = CMD_TOPIC;
const char bedroom_temp_topic[] = BEDROOM_TEMP_TOPIC;
const char livingroom_temp_topic[] = LIVINGROOM_TEMP_TOPIC;
const char outside_temp_topic[] = OUTSIDE_TEMP_TOPIC;
const char status_topic[] = STATUS_TOPIC;

const char* temperatureIdxToString[] = { "Bedroom", "Living room", "Center", "Outside" };


#define setValue(func)        thermoController.func; thermostatWi.func;


void updateSensorDataByIdx(TemperatureIdx idx, const char* payload)
{
    char* pos = strstr(payload, "temp");
    float temperatute;
    int humidity = -1;

    if (pos != nullptr)
    {
        temperatute = atof(pos + 5);
        thermoController.updateTemperature(idx, temperatute);
    }
    pos = strstr(payload, "hum");
    if (pos != nullptr)
    {
        humidity = atoi(pos + 4);
    }
    thermostatWi.updateTemperatureAndHumidity(idx, temperatute, humidity);
}


void CmdManager_c::ProcessMqttPayload(const char* topic, const char* payload) const
{
    if (strncmp(topic, cmd_topic, strlen(cmd_topic)) == 0)
    {
        if (strncmp(payload, "on", 2) == 0)
        {
            setValue(setEnabled(true));
        }
        else if (strncmp(payload, "off", 3) == 0)
        {
            setValue(setEnabled(false));
        }
        else if (strncmp(payload, "max_temp=", 9) == 0)
        {
            const float new_max = atof(payload + 9);
            setValue(setMaxTemperature(new_max));
        }
        else if (strncmp(payload, "min_temp=", 9) == 0)
        {
            const float new_min = atof(payload + 9);
            setValue(setMinTemperature(new_min));
        }
        else if (strncmp(payload, "mode=night", 10) == 0)
        {
            setValue(setMode(NIGHT_MODE));
        }
        else if (strncmp(payload, "mode=normal", 11) == 0)
        {
            setValue(setMode(NORMAL_MODE));
        }
        else if (strncmp(payload, "mode=bedroomClose", 11) == 0)
        {
            setValue(setMode(NORMAL_MODE));
        }
    }
    else if (strncmp(topic, bedroom_temp_topic, strlen(bedroom_temp_topic)) == 0)
    {
        updateSensorDataByIdx(BEDROOM_IDX, payload);
    }
    else if (strncmp(topic, livingroom_temp_topic, strlen(livingroom_temp_topic)) == 0)
    {
        updateSensorDataByIdx(LIVINGROOM_IDX, payload);
    }
    else if (strncmp(topic, outside_temp_topic, strlen(outside_temp_topic)) == 0)
    {
        updateSensorDataByIdx(OUTSIDE_IDX, payload);
    }
}

