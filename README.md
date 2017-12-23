# Thermostat based on ESP32
Thermostat controlled though touch LCD and MQTT over WiFi

![Front pannel](Photo/20171222_140539.jpg?raw=true "Front pannel")
![Inside](Photo/20171222_140600.jpg?raw=true "Inside")
This thermostat I have created to integrate climate facility into my smart home system. Particularly I use openhab server on my raspberry pi as smart home brain. But it may be any other system which supports MQTT protocol. This thermostat also can be used as standalone.

## Hardware
1. [ESP32 Development Board](https://www.aliexpress.com/item/MH-ET-LIVE-ESP32-Development-Board-WiFi-Bluetooth-Ultra-Low-Power-Consumption-Dual-Core-ESP-32/32817818506.html)
2. [LCD Touch Panel with PCB ILI9341](https://www.aliexpress.com/item/1pcs-J34-F85-240x320-2-8-SPI-TFT-LCD-Touch-Panel-Serial-Port-Module-with-PCB/32795636902.html)
3. [3 Channel Relay](https://www.aliexpress.com/item/3-three-channel-3-road-high-voltage-relay-module-with-optical-coupling-isolation-fully-compatible/32616653345.html)
4. [DHT11 Temperature/Humidity Sensor](https://www.aliexpress.com/item/DHT11-DHT-11-Digital-Temperature-and-Humidity-Sensor/2038557639.html)
5. Any 5V power source
6. Remote temperature sensors publishing data to MQTT. I use same DHT11 + NodeMcu (ESP8266) like [here](https://github.com/denmatfoton/NodeMcuMotionDetector)

## Software
This sources are compilled though [GNU toolchain for esp32](http://gnutoolchains.com/esp32/) using Makefiles.

ESP32 core is used from [Arduino core for ESP32](https://github.com/espressif/arduino-esp32)
Project uses [UGFX library](https://ugfx.io/) which is included. gos_freertos was sightly modified to meet FreeRTOS API for ESP32. Also was modified ADS7843 driver and implemented communication API.
Sources are dependent on several Arduino libraries (Wifi PubSubClient ArduinoOTA ESPmDNS Update SPI Adafruit_Unified_Sensor DHT_sensor_library OneWire DallasTemperature). They should be installed through Arduino IDE or downloaded directly from git.
Pathes should be specified in esp32config.mk and common.mk.

Compile: make all
Flash: make all flash
Make is from toolchain.
