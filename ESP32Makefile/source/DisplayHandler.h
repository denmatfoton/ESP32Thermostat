#pragma once
#include "CustomWidgets.h"
#include "DisplayClock.h"
#include "ThermostatController.h"


void DisplaySetup();


class ThermostatWidget
{
public:
    ThermostatWidget();
    void createThermostatWidget(void);
    void processEvent(GEvent* pe);
    void setTargetTemperature(int val) { targetTempInput.setValue(val); }
    /*void updateTemperature(const uint8_t idx, const float val);
    void updateHumidity(const uint8_t idx, const float val) {}*/
    void updateTemperatureAndHumidity(const uint8_t idx, const float temp, const int hum);
    void updateStyle();
    //void updateState(RelayState state);
    void setEnabled(bool enabled) { thermostatEnabledInput.setValue(enabled); }
    void setFanEnabled(bool enabled) { fanEnabledInput.setValue(enabled); }
    void setMode(ThermostatMode mode) { thermostatModeInput.setValue(mode); updateStyle(); }

    friend void inputCallback(Input* input);

private:
    void createTempLabels(GWidgetInit &wi);

    NumberInput                  targetTempInput;
    Switch                       thermostatEnabledInput;
    Switch                       fanEnabledInput;
    Radio<ThermostatMode, 3>     thermostatModeInput;
    DisplayClock                 clock;

    GHandle      ghContainer;
    GHandle      ghTemperatureContainer;
    GHandle      ghTempLabels[TEMPR_COUNT];
};


extern ThermostatWidget thermostatWi;
