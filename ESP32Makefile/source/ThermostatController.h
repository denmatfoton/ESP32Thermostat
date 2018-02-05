#pragma once
#include <Arduino.h>
#include <stdint.h>
#include "cfg.h"
#include "CmdHandler.h"


enum RelayState
{
    NONE,
    CONDITIONING_ENABLED,
    HEATING_ENABLED
};


class ThermostatController
{
public:
    ThermostatController();

    void Process();

    void setEnabled(bool val);
    void setMode(const ThermostatMode mode);
    void setFanEnabled(bool val);
    void setTargetTemperature(const float value);

    float getTargetTemperature() { return target_temperature; }
    void updateTemperature(const uint8_t idx, const float value);

    bool getEnabled() const { return enabled; }
    float getAverageTemperature() { return average_current_temperature; }
    float getTermoValue(const uint8_t idx) const { return termo_values[idx]; }
    ThermostatMode getThermoMode() const { return thermo_mode; }
private:
    void setRelayState(const RelayState state);
    void checkActuality();
    void checkEnableCondition(bool heating, float max_current_temperature, float min_current_temperature);

    portMUX_TYPE mutex;
    float target_temperature;
    float average_current_temperature = 0;
    bool enabled = false;
    float termo_values[TEMPR_COUNT];
    uint8_t termo_coeff[TERMO_COUNT];
    TickType_t termo_update_time[TEMPR_COUNT];
    RelayState current_state = NONE;
    RelayState last_enabled_state = NONE;
    ThermostatMode thermo_mode = NORMAL_MODE;
};


extern ThermostatController thermoController;
