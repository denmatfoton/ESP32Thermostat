#include "ThermostatController.h"
#include "Arduino.h"
#include <assert.h>
#include <string.h>


ThermostatController thermoController;
#define MSG_MAX_LENGTH         20
static char msg[MSG_MAX_LENGTH];
#define UNDEFINED_TEMP_VALUE       -1e10f


ThermostatController::ThermostatController()
{
    min_temperature = 19;
    max_temperature = 25;

    vPortCPUInitializeMutex(&mutex);
    for (int i = 0; i < TERMO_COUNT; ++i)
    {
        termo_values[i] = UNDEFINED_TEMP_VALUE;
    }
    memset(termo_coeff, 1, sizeof(termo_coeff));

    pinMode(HEATING_PIN, OUTPUT);
    pinMode(CONDITIONING_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);

    digitalWrite(HEATING_PIN, LOW);
    digitalWrite(CONDITIONING_PIN, LOW);
    digitalWrite(FAN_PIN, LOW);
}


void ThermostatController::process()
{
    if (!enabled)
    {
        setRelayState(NONE);
        return;
    }

    float max_current_temperature = UNDEFINED_TEMP_VALUE;
    float min_current_temperature = UNDEFINED_TEMP_VALUE;
    float average_current_temperature = 0;
    uint16_t coeff_sum = 0;

    for (int i = 0; i < TERMO_COUNT; ++i)
    {
        if (termo_values[i] == UNDEFINED_TEMP_VALUE)
        {
            continue;
        }
        if (termo_values[i] > max_current_temperature || max_current_temperature == UNDEFINED_TEMP_VALUE)
        {
            max_current_temperature = termo_values[i];
        }
        if (termo_values[i] < min_current_temperature || min_current_temperature == UNDEFINED_TEMP_VALUE)
        {
            min_current_temperature = termo_values[i];
        }
        average_current_temperature += termo_values[i] * termo_coeff[i];
        coeff_sum += termo_coeff[i];
    }
    if (max_current_temperature == UNDEFINED_TEMP_VALUE)
    {
        return;
    }

    average_current_temperature /= coeff_sum;
    Serial.printf("Average: %f, min: %f, max: %f\n", average_current_temperature, min_temperature, max_temperature);

    switch (current_state)
    {
    case  HEATING_ENABLED:
        if (average_current_temperature > min_temperature + TEMPR_CUSHION ||
            max_current_temperature > max_temperature)
        {
            setRelayState(NONE);
        }
        break;
    case  CONDITIONING_ENABLED:
        if (average_current_temperature < max_temperature - TEMPR_CUSHION ||
            min_current_temperature < min_temperature)
        {
            setRelayState(NONE);
        }
        break;
    case NONE:
        if (average_current_temperature < min_temperature &&
            max_current_temperature < max_temperature)
        {
            setRelayState(HEATING_ENABLED);
        }
        else if (average_current_temperature > max_temperature &&
            min_current_temperature > min_temperature)
        {
            setRelayState(CONDITIONING_ENABLED);
        }
        break;
    default:
        break;
    }
}


void ThermostatController::setEnabled(bool val)
{
    enabled = val;
    if (enabled)
    {
        SendMqtt(status_topic, "on");
    }
    else
    {
        SendMqtt(status_topic, "off");
    }
}

void ThermostatController::setMinTemperature(const float value)
{
    taskENTER_CRITICAL(&mutex);
    min_temperature = value;
    taskEXIT_CRITICAL(&mutex);
    snprintf(msg, MSG_MAX_LENGTH, "{min_temp:%.1f}", value);
    SendMqtt(status_topic, msg);
}

void ThermostatController::setMaxTemperature(const float value)
{
    taskENTER_CRITICAL(&mutex);
    max_temperature = value;
    taskEXIT_CRITICAL(&mutex);
    snprintf(msg, MSG_MAX_LENGTH, "{max_temp:%.1f}", value);
    SendMqtt(status_topic, msg);
}


void ThermostatController::setMode(const ThermostatMode mode)
{
    thermo_mode = mode;

    switch (thermo_mode)
    {
    case NORMAL_MODE:
        memset(termo_coeff, 1, TERMO_COUNT);
        SendMqtt(status_topic, "{mode:normal}");
        break;
    case NIGHT_MODE:
        memset(termo_coeff, 0, TERMO_COUNT);
        termo_coeff[BEDROOM_IDX] = 1;
        SendMqtt(status_topic, "{mode:night}");
        break;
    case BEDROOM_CLOSE_MODE:
        memset(termo_coeff, 1, TERMO_COUNT);
        termo_coeff[BEDROOM_IDX] = 0;
        SendMqtt(status_topic, "{mode:bedroomClose}");
        break;
    }
}


void ThermostatController::setFanEnabled(bool val)
{
    if (val)
    {
        digitalWrite(FAN_PIN, HIGH);
    }
    else
    {
        digitalWrite(FAN_PIN, LOW);
    }
}


void ThermostatController::setRelayState(const RelayState state)
{
    if (current_state != state)
    {
        switch (current_state)
        {
        case  HEATING_ENABLED:
            digitalWrite(HEATING_PIN, LOW);
            break;
        case  CONDITIONING_ENABLED:
            digitalWrite(CONDITIONING_PIN, LOW);
            break;
        default:
            break;
        }
        switch (state)
        {
        case  HEATING_ENABLED:
            digitalWrite(HEATING_PIN, HIGH);
            break;
        case  CONDITIONING_ENABLED:
            digitalWrite(CONDITIONING_PIN, HIGH);
            break;
        default:
            break;
        }
        current_state = state;
        Serial.printf("RelayState: %d\n", static_cast<int>(state));
    }
}
