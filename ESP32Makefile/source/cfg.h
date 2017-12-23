#ifndef CFG_H
#define CFG_H
#include <stdint.h>

//Pins assignment
#define HEATING_PIN          25
#define CONDITIONING_PIN     26
#define FAN_PIN              27

#define ONE_WIRE_PIN         33    // for termo sensor

// TFT + Touch Screen Setup Start
#define TFT_DC_PIN           4
#define TFT_CS_PIN           5
#define TFT_RST_PIN          16

#define TOUCH_CS_PIN         17
#define TOUCH_IRQ_PIN        15

// SPI Pins are shared by TFT, touch screen
#define SPI_MISO_PIN         19
#define SPI_MOSI_PIN         23
#define SPI_CLK_PIN          18


#define UART_BAUD_RATE    115200

//WiFi setup
#define CLIENT_NAME       "ESP32_Thermostat"
#define SSID_TO_CONNECT   "SSID"
#define WIFI_PASS         "Password"

#define OTA_PASS          "UpdatePassword"
#define OTA_PORT          8266

//MQTT setup
#define MQTT_SERVER       "192.168.0.18"
#define MQTT_PORT         1883
#define MQTT_USER         "mqtt_user"
#define MQTT_PASSWORD     "MQTT_password"

//MQTT topics
#define MAX_PAYLOAD_LENGTH     100
#define CMD_TOPIC              "myhome/thermostat"
#define BEDROOM_TEMP_TOPIC     "myhome/bedroom/temphum"
#define LIVINGROOM_TEMP_TOPIC  "myhome/livingroom/temphum"
#define OUTSIDE_TEMP_TOPIC     "myhome/outside/temp"
#define CENTER_TEMP_TOPIC      "myhome/center/temp"
#define STATUS_TOPIC           "myhome/thermostat/status"

extern const char cmd_topic[];
extern const char bedroom_temp_topic[];
extern const char livingroom_temp_topic[];
extern const char outside_temp_topic[];
extern const char status_topic[];

void SendMqtt(const char* topic, const char* response);


#define TERMO_COUNT          3      // inside the house
#define TEMPR_COUNT          4      // all sensors (inside + outside)
#define HUM_COUNT            TERMO_COUNT
#define TEMPR_CUSHION        1.0f   // heat/cool 1 degree more/less to avoid too offen enabling/disabling of climat facility

#endif // CFG_H
