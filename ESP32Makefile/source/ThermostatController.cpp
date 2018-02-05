#include "ThermostatController.h"
#include "Arduino.h"
#include <assert.h>
#include <string.h>


ThermostatController thermoController;
#define MSG_MAX_LENGTH             20
#define UNDEFINED_TEMP_VALUE       -1e10f
#define SEND_RETAIN_TO_TOPIC       0

void SendMqtt(const char* topic, const char* response, bool retain = false);


ThermostatController::ThermostatController() : target_temperature(21.f)
{
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


void ThermostatController::Process()
{
    float max_current_temperature = UNDEFINED_TEMP_VALUE;
    float min_current_temperature = UNDEFINED_TEMP_VALUE;
    float S = 0;
    uint16_t coeff_sum = 0;

    checkActuality();

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
        S += termo_values[i] * termo_coeff[i];
        coeff_sum += termo_coeff[i];
    }
    if (max_current_temperature == UNDEFINED_TEMP_VALUE)
    {
        setRelayState(NONE);
        return;
    }

    average_current_temperature = S / coeff_sum;

    if (!enabled)
    {
        setRelayState(NONE);
        return;
    }

    
    switch (current_state)
    {
    case  HEATING_ENABLED:
        if (average_current_temperature >= target_temperature ||
            max_current_temperature >= target_temperature + TEMPR_CUSHION_MAX)
        {
            setRelayState(NONE);
        }
        break;
    case  CONDITIONING_ENABLED:
        if (average_current_temperature <= target_temperature ||
            min_current_temperature < target_temperature - TEMPR_CUSHION_MAX)
        {
            setRelayState(NONE);
        }
        break;
    case NONE:
        if (termo_values[OUTSIDE_IDX] != UNDEFINED_TEMP_VALUE)
        {
            checkEnableCondition(termo_values[OUTSIDE_IDX] < target_temperature, min_current_temperature, max_current_temperature);
        }
        else
        {
            if (last_enabled_state == NONE)
            {
                checkEnableCondition(average_current_temperature < target_temperature, min_current_temperature, max_current_temperature);
            }
            else
            {
                checkEnableCondition(last_enabled_state == HEATING_ENABLED, min_current_temperature, max_current_temperature);
            }
        }
        break;
    default:
        break;
    }
}


void ThermostatController::checkEnableCondition(bool heating, float min_current_temperature, float max_current_temperature)
{
    if (heating)
    {
        if (average_current_temperature < target_temperature - TEMPR_CUSHION_MIN &&
            max_current_temperature < target_temperature + TEMPR_CUSHION_MAX)
        {
            setRelayState(HEATING_ENABLED);
        }
    }
    else
    {
        if (average_current_temperature > target_temperature + TEMPR_CUSHION_MIN &&
            min_current_temperature > target_temperature - TEMPR_CUSHION_MAX)
        {
            setRelayState(CONDITIONING_ENABLED);
        }
    }
}


void ThermostatController::setEnabled(bool val)
{
    char* msg;

    if (val)
    {
        msg = "on";
    }
    else
    {
        msg = "off";
    }

    SendMqtt(POWER_STATE_TOPIC, msg);

    if (enabled != val)
    {
        enabled = val;
        last_enabled_state = NONE;
#if SEND_RETAIN_TO_TOPIC
        SendMqtt(POWER_TOPIC, msg, true);
#endif
    }
}


void ThermostatController::setTargetTemperature(const float value)
{
    static char msg[MSG_MAX_LENGTH];

    snprintf(msg, MSG_MAX_LENGTH, "%.1f", value);
    SendMqtt(TARGET_TEMP_STATE_TOPIC, msg);
    if (target_temperature != value)
    {
        target_temperature = value;
#if SEND_RETAIN_TO_TOPIC
        SendMqtt(TARGET_TEMP_TOPIC, msg, true);
#endif
    }
}


void ThermostatController::updateTemperature(const uint8_t idx, const float value)
{
    termo_values[idx] = value;
    termo_update_time[idx] = xTaskGetTickCount();
}


void ThermostatController::setMode(const ThermostatMode mode)
{
    char* msg;

    switch (mode)
    {
    case NORMAL_MODE:
        memset(termo_coeff, 1, TERMO_COUNT);
        msg = "normal";
        break;
    case NIGHT_MODE:
        memset(termo_coeff, 0, TERMO_COUNT);
        termo_coeff[BEDROOM_IDX] = 1;
        msg = "night";
        break;
    case BEDROOM_OFF_MODE:
        memset(termo_coeff, 1, TERMO_COUNT);
        termo_coeff[BEDROOM_IDX] = 0;
        msg = "bedroom_off";
        break;
    }


    SendMqtt(MODE_STATE_TOPIC, msg);
    if (thermo_mode != mode)
    {
        thermo_mode = mode;
#if SEND_RETAIN_TO_TOPIC
        SendMqtt(MODE_TOPIC, msg, true);
#endif
    }
}


void ThermostatController::setFanEnabled(bool val)
{
    if (val)
    {
        digitalWrite(FAN_PIN, HIGH);
        SendMqtt(POWER_STATE_TOPIC, "enabled");
    }
    else
    {
        digitalWrite(FAN_PIN, LOW);
        SendMqtt(POWER_STATE_TOPIC, "disabled");
    }
}


void ThermostatController::setRelayState(const RelayState state)
{
    if (current_state != state)
    {
        switch (current_state)
        {
        case  HEATING_ENABLED:
            last_enabled_state = current_state;
            digitalWrite(HEATING_PIN, LOW);
            break;
        case  CONDITIONING_ENABLED:
            last_enabled_state = current_state;
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


void ThermostatController::checkActuality()
{
    TickType_t cur_tick = xTaskGetTickCount();

    for (int i = 0; i < TERMO_COUNT; ++i)
    {
        if ((cur_tick - termo_update_time[i]) / configTICK_RATE_HZ > TEMPR_ACTUALITY_TIME)
        {
            termo_values[i] = UNDEFINED_TEMP_VALUE;
        }
    }
}

