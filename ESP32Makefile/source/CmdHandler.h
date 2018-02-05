#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H
#include "cfg.h"


enum TemperatureIdx
{
    BEDROOM_IDX = 0,
    LIVINGROOM_IDX,
    CENTER_IDX,
    OUTSIDE_IDX
};

extern const char* temperatureIdxToString[];
#define getTemperatureName(idx)       temperatureIdxToString[idx]

enum ThermostatMode
{
    NORMAL_MODE = 0,
    NIGHT_MODE,
    BEDROOM_OFF_MODE
};


class CmdManager_c
{
public:
    CmdManager_c(void) {}
    void ProcessMqttPayload(const char* topic, const char* payload) const;
private:
};

extern CmdManager_c CmdManager;


#endif // CMD_HANDLER_H
