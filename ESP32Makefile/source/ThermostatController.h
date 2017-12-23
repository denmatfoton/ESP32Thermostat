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

    void process();

    void setEnabled(bool val);
    void setMinTemperature(const float value);
    void setMaxTemperature(const float value);
    void updateTemperature(const uint8_t idx, const float value) { termo_values[idx] = value; }
    void setMode(const ThermostatMode mode);

    bool getEnabled() const { return enabled; }
    float getMinTemperature() { return min_temperature; }
    float getMaxTemperature() { return max_temperature; }
    float getTermoValue(const uint8_t idx) const { return termo_values[idx]; }
    ThermostatMode getThermoMode() const { return thermo_mode; }
    void setFanEnabled(bool val);
private:
    void setRelayState(const RelayState state);

    portMUX_TYPE mutex;
    float min_temperature;
    float max_temperature;
    bool enabled = false;
    float termo_values[TEMPR_COUNT];
    uint8_t termo_coeff[TERMO_COUNT];
    RelayState current_state = NONE;
    ThermostatMode thermo_mode = NORMAL_MODE;
};


extern ThermostatController thermoController;
