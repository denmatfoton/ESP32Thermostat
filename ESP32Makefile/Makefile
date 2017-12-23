include common.mk
include esp32config.mk

#Additional flags
INCLUDE_DIRS := $(ESP32_INCLUDE_DIRS)
LIBRARY_DIRS := $(ESP32_LIBRARY_INCLUDE)
LIBRARY_NAMES := $(ESP32_LIBS)
ADDITIONAL_LINKER_INPUTS := 

CFLAGS := $(ESP32_CFLAGS)
CXXFLAGS := $(ESP32_CXXFLAGS)
LDFLAGS := $(ESP32_LDFLAGS)


TARGETNAME := ESP32Thermostat

PARTITIONS_BIN_PATH := $(BINARYDIR)/$(TARGETNAME).partitions.bin
MAIN_BIN_PATH := $(BINARYDIR)/$(TARGETNAME).bin
ELF_BIN_PATH := $(BINARYDIR)/$(TARGETNAME).elf


ALL_BINS_TO_FLASH := $(FLASH_BOOT_APP) $(FLASH_BOOTLOADER) \
						$(MAIN_BIN_OFFSET) $(MAIN_BIN_PATH) \
						$(PARTITIONS_BIN_OFFSET) $(PARTITIONS_BIN_PATH)


SRCDIR := source
SOURCEFILES := $(wildcard $(SRCDIR)/*.c)
SOURCEFILES := $(wildcard $(SRCDIR)/*.cpp)



SELECTED_ARDUINO_LIBS := Wifi PubSubClient ArduinoOTA ESPmDNS Update SPI Adafruit_Unified_Sensor DHT_sensor_library OneWire DallasTemperature

SELECTED_ARDUINO_LIBS_PATH := $(foreach lib,$(SELECTED_ARDUINO_LIBS),$(strip $(call find_arduino_lib,$(lib))))

ADDITIONAL_INCLUDES := $(SELECTED_ARDUINO_LIBS_PATH)
ADDITIONAL_INCLUDES += $(foreach dir,$(SELECTED_ARDUINO_LIBS_PATH),$(wildcard $(dir)/src))

INCLUDE_DIRS += $(ADDITIONAL_INCLUDES)
export ADDITIONAL_INCLUDES


#add source directories to path
SOURCE_DIRS := $(dir $(SOURCEFILES))
VPATH = $(sort $(SOURCE_DIRS))

INCLUDE_DIRS += $(SRCDIR)
INCLUDE_DIRS += $(file <gfx.inc)


CFLAGS += $(addprefix -I,$(INCLUDE_DIRS))
CXXFLAGS += $(addprefix -I,$(INCLUDE_DIRS))

CFLAGS += $(addprefix -D,$(PREPROCESSOR_MACROS))
CXXFLAGS += $(addprefix -D,$(PREPROCESSOR_MACROS))

CXXFLAGS += $(addprefix -framework ,$(MACOS_FRAMEWORKS))
CFLAGS += $(addprefix -framework ,$(MACOS_FRAMEWORKS))
LDFLAGS += $(addprefix -framework ,$(MACOS_FRAMEWORKS))

LDFLAGS += $(addprefix -L,$(LIBRARY_DIRS))

LIBRARY_LDFLAGS = $(addprefix -l,$(LIBRARY_NAMES))


source_obj1 := $(SOURCEFILES:.cpp=.o)
source_objs := $(source_obj1:.c=.o)

all_objs := $(addprefix $(OBJDIR)/, $(notdir $(source_objs)))


TARGET_COMPONENTS := $(ESP32_CORE_TARGET)
TARGET_COMPONENTS += $(foreach lib,$(SELECTED_ARDUINO_LIBS),$(strip $(call get_lib_target,$(lib))))
TARGET_COMPONENTS += $(strip $(call get_lib_target,gfx))


all: create_main_dirs esp32core arduino_libs gfx_lib $(MAIN_BIN_PATH) $(PARTITIONS_BIN_PATH)


$(MAIN_BIN_PATH): $(ELF_BIN_PATH)
	$(ESPTOOL) $(ESPTOOL_LINK_PARAMS) -o $@ $<

$(PARTITIONS_BIN_PATH):
	$(GEN_ESP32PART) -q $(PARTITION_TABLE_CSV_PATH) $@

$(ELF_BIN_PATH): $(all_objs) esp32core arduino_libs gfx_lib
	$(LD) $(LDFLAGS) $(START_GROUP) $(all_objs) $(TARGET_COMPONENTS) $(LIBRARY_LDFLAGS) $(END_GROUP) -Wl,-EL -o $@ 

esp32core:
	$(MAKE) -f esp32core.mk

arduino_libs:
	for lib in $(SELECTED_ARDUINO_LIBS) ; do \
		$(MAKE) -f compileArduinoLib.mk LIB_NAME=$$lib ; \
    done

gfx_lib:
	$(MAKE) -f ugfx.mk


flash: all
	$(ESPTOOL) $(ESPTOOL_FLASH_PARAMS) $(ALL_BINS_TO_FLASH)


clean:
	rm -rf $(BINARYDIR)
	rm -rf $(OBJDIR)

create_main_dirs:
	mkdir -p $(BINARYDIR)
	mkdir -p $(OBJDIR)


$(OBJDIR)/%.o : %.cpp
	@echo
	@echo $(MSG_COMPILING_CPP) $<
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) $(CFLAGS) -c $< -o $@
