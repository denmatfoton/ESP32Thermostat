ESP32_PATH := C:/Users/P51/Documents/Arduino/hardware/espressif/esp32
ESP32_CORE_PATH := $(ESP32_PATH)/cores/esp32
ARDUINO_CORE_LIBS_PATH := $(ESP32_PATH)/libraries
ARDUINO_USER_LIBS_PATH := C:/Users/P51/Documents/Arduino/libraries
ARDUINO_LIBS_PATH := $(ARDUINO_CORE_LIBS_PATH) $(ARDUINO_USER_LIBS_PATH)


ESP32_OBJDIR := $(OBJDIR)/core


BOOT_APP_BIN_OFFSET := 0xe000
BOOT_APP_BIN_PATH := $(ESP32_PATH)/tools/partitions/boot_app0.bin
FLASH_BOOT_APP := $(BOOT_APP_BIN_OFFSET) $(BOOT_APP_BIN_PATH)

BOOTLOADER_BIN_OFFSET := 0x1000
BOOTLOADER_BIN_PATH := $(ESP32_PATH)/tools/sdk/bin/bootloader_qio_80m.bin
FLASH_BOOTLOADER := $(BOOTLOADER_BIN_OFFSET) $(BOOTLOADER_BIN_PATH)

PARTITIONS_BIN_OFFSET := 0x8000

MAIN_BIN_OFFSET := 0x10000


ifndef CONFIG_PARTITION_TABLE_CUSTOM
PARTITION_TABLE_CSV_PATH = $(ESP32_PATH)/tools/partitions/default.csv
endif


GEN_ESP32PART := $(ESP32_PATH)/tools/gen_esp32part.exe
ESPTOOL := $(ESP32_PATH)/tools/esptool.exe
ESPOTATOOL := $(ESP32_PATH)/tools/espota.exe
ESPOTA_PASSOWORD := UPDATE_PW


ESPTOOL_LINK_PARAMS := --chip esp32 elf2image --flash_mode "dio" --flash_freq "80m" --flash_size "4MB"
ESPTOOL_FLASH_PARAMS := --chip esp32 --port "COM13" --baud 921600 --before default_reset --after hard_reset \
						write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect

ESP32_CORE_TARGET := $(ESP32_OBJDIR)/esp32core.a


ESP32_LIBRARY_INCLUDE := $(ESP32_PATH)/tools/sdk/lib \
							$(ESP32_PATH)/tools/sdk/ld \


ESP32_LIBS := gcc openssl btdm_app fatfs wps coexist wear_levelling hal newlib \
				driver bootloader_support pp smartconfig jsmn wpa ethernet phy app_trace \
				console ulp wpa_supplicant freertos bt micro-ecc cxx xtensa-debug-module \
				mdns vfs soc core sdmmc coap tcpip_adapter c_nano rtc spi_flash wpa2 \
				esp32 app_update nghttp spiffs espnow nvs_flash esp_adc_cal log expat \
				m c heap mbedtls lwip net80211 pthread json stdc++


ESP32_LDFLAGS := -nostdlib
ESP32_LDFLAGS += -T esp32_out.ld -T esp32.common.ld -T esp32.rom.ld -T esp32.peripherals.ld -T esp32.rom.spiram_incompatible_fns.ld
ESP32_LDFLAGS += -u ld_include_panic_highint_hdl -u call_user_start_cpu0
ESP32_LDFLAGS += -Wl,--gc-sections -Wl,-static -Wl,--undefined=uxTopUsedPriority
ESP32_LDFLAGS += -u __cxa_guard_dummy -u __cxx_fatal_exception



ESP32_COMMONFLAGS := -Os -g3 -fstack-protector -ffunction-sections -fdata-sections \
					-fstrict-volatile-bitfields -mlongcalls -nostdlib -Wpointer-arith -w \
					-Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable \
					-Wno-error=deprecated-declarations -Wno-unused-parameter -Wno-sign-compare -MMD

ESP32_CFLAGS := -std=gnu99 -Wno-old-style-declaration $(ESP32_COMMONFLAGS)
ESP32_CXXFLAGS := -std=gnu++11 -fno-exceptions -fno-rtti $(ESP32_COMMONFLAGS)


ESP32_INCLUDE_DIRS := $(sort $(dir $(wildcard $(ESP32_PATH)/tools/sdk/include/*/)))
ESP32_INCLUDE_DIRS += $(ESP32_PATH)/tools/sdk/include/freertos/freertos
ESP32_INCLUDE_DIRS += $(ESP32_CORE_PATH) \
						$(ESP32_PATH)/variants/esp32
						
						
find_arduino_lib = $(foreach dir,$(ARDUINO_LIBS_PATH),$(wildcard $(dir)/$1))

get_lib_target = $(OBJDIR)/$1/$1.a
