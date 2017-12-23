#include <Arduino.h>
#include "ThermoHandler.h"
#include "ThermostatController.h"
#include "DisplayHandler.h"
#include "cfg.h"


void addFloatToStr(char* str, const float f, uint8_t num_of_dig_after_point)
{
    uint16_t fraction;
    uint16_t devider = 1;

    while (num_of_dig_after_point--)
    {
        devider *= 10;
    }

    if (f >= 0)
    {
        fraction = static_cast<uint16_t>(f * devider) % devider;
    }
    else
    {
        fraction = static_cast<uint16_t>(-f * devider) % devider;
    }

    sprintf(str + strlen(str), "%d.%d", static_cast<int>(f), fraction);
}


void Thermometer::sendTemperature(const char* topic)
{
    static char msg[20];
    float cur_temp;
    int hum;

    memcpy(msg, "{temp:", 8);
    if ((error_code = getTemperature(&cur_temp)) != STATUS_OK)
    {
        sprintf(msg, "error=%d", error_code);
        SendMqtt(topic, msg);
        return;
    }

    addFloatToStr(msg, cur_temp, 1);
    strcat(msg, "}");

    SendMqtt(topic, msg);
}


Thermometer_DS1820::Thermometer_DS1820(const uint8_t pin) : oneWireBus(pin), instance()
{
    instance.setOneWire(&oneWireBus);
}


void Thermometer_DS1820::initialize()
{
    instance.begin();
}


Status Thermometer_DS1820::getTemperature(float* value)
{
    instance.setResolution(9);
    instance.requestTemperaturesByIndex(0);
    *value = instance.getTempCByIndex(0);

    if (*value == 85.0 || *value == (-127.0))
    {
        return CONNECTION_BROKEN;
    }

    return STATUS_OK;
}


Thermometer_DHT11::Thermometer_DHT11(uint8_t pin) : instance(ONE_WIRE_PIN, DHT11)
{
}


void Thermometer_DHT11::sendTemperature(const char* topic)
{
    static char msg[30];
    float cur_temp;
    uint8_t cur_hum;

    if ((error_code = getTemperature(&cur_temp)) != STATUS_OK)
    {
        sprintf(msg, "error=%d", error_code);
        SendMqtt(topic, msg);
        return;
    }
    if ((error_code = getHumidity(&cur_hum)) != STATUS_OK)
    {
        sprintf(msg, "error=%d", error_code);
        SendMqtt(topic, msg);
        return;
    }

    snprintf(msg, 30, "{temp:%.1f,hum:%d}", cur_temp, cur_hum);

    SendMqtt(topic, msg);
}


Status Thermometer_DHT11::getTemperature(float * value)
{
    // Read temperature as Celsius (the default)
    //instance.
    *value = instance.readTemperature();

    if (*value > 85.0 || *value < (-50.0))
    {
        return error_code = CONNECTION_BROKEN;
    }

    return error_code = STATUS_OK;
}


Status Thermometer_DHT11::getHumidity(uint8_t * value)
{
    *value = static_cast<uint8_t>(instance.readHumidity());

    if (*value > 100)
    {
        return error_code = CONNECTION_BROKEN;
    }

    return error_code = STATUS_OK;
}


void Thermometer_DHT11::initialize()
{
    instance.begin();
}



static TaskHandle_t termometerTaskHandle;
static Thermometer_DHT11 dht11(ONE_WIRE_PIN);
static EventGroupHandle_t xEventGroup = NULL;
#define TEMPERATURE_READY_EVENT_BIT    (1 << 0)

static float currentTemperature;
static uint8_t currentHumidity;
//static SemaphoreHandle_t termoMutex;

static void TermometerTask(void* param);


void TermometerSetup()
{
    //termoMutex = xSemaphoreCreateMutex();
    xEventGroup = xEventGroupCreate();

    xTaskCreate(TermometerTask, "TermometerTask", configMINIMAL_STACK_SIZE + 256, NULL, 1, &termometerTaskHandle);
}


void CheckTemperatue()
{
    static char msg[30];
    EventBits_t uxBits;

    uxBits = xEventGroupWaitBits(
        xEventGroup,   /* The event group being tested. */
        TEMPERATURE_READY_EVENT_BIT, /* The bits within the event group to wait for. */
        pdTRUE,        /* bit should be cleared before returning. */
        pdFALSE,       /* Don't wait for all bits, either bit will do. */
        10);           /* Wait a maximum of 10ms for either bit to be set. */

    if ((uxBits & TEMPERATURE_READY_EVENT_BIT) != 0)
    {
        if (dht11.getLastStaus() == STATUS_OK)
        {
            snprintf(msg, 30, "{temp:%.1f,hum:%d}", currentTemperature, currentHumidity);
            SendMqtt(CENTER_TEMP_TOPIC, msg);

            thermoController.updateTemperature(CENTER_IDX, currentTemperature);
            thermostatWi.updateTemperatureAndHumidity(CENTER_IDX, currentTemperature, currentHumidity);
        }
        else
        {
            Serial.println("Termo error");
        }
    }
}


static void TermometerTask(void* param)
{
    dht11.initialize();

    while (1)
    {
        Status error_code;
        vTaskDelay(10000);

        dht11.getTemperature(&currentTemperature);
        dht11.getHumidity(&currentHumidity);

        xEventGroupSetBits(xEventGroup, TEMPERATURE_READY_EVENT_BIT);
    }
}
