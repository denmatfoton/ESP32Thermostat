BINARYDIR := bin
OBJDIR := obj

#Toolchain
TOOLCHAIN_ROOT := C:/SysGCC/esp32

CC := $(TOOLCHAIN_ROOT)/bin/xtensa-esp32-elf-gcc.exe
CXX := $(TOOLCHAIN_ROOT)/bin/xtensa-esp32-elf-g++.exe
LD := $(CC)
AR := $(TOOLCHAIN_ROOT)/bin/xtensa-esp32-elf-ar.exe
OBJCOPY := $(TOOLCHAIN_ROOT)/bin/xtensa-esp32-elf-objcopy.exe
MAKE := $(TOOLCHAIN_ROOT)/bin/make.exe


START_GROUP := -Wl,--start-group
END_GROUP := -Wl,--end-group


# Define Messages
# English
MSG_ERRORS_NONE = Errors: none
MSG_BEGIN = -------- begin --------
MSG_END = --------  end  --------
MSG_SIZE_BEFORE = Size before: 
MSG_SIZE_AFTER = Size after:
MSG_FLASH = Creating load file for Flash:
MSG_EXTENDED_LISTING = Creating Extended Listing:
MSG_SYMBOL_TABLE = Creating Symbol Table:
MSG_LINKING = Linking:
MSG_COMPILING = Compiling C:
MSG_COMPILING_CPP = Compiling C++:
MSG_ASSEMBLING = Assembling:
MSG_CLEANING = Cleaning project:
MSG_CREATING_LIBRARY = Creating library:


PREPROCESSOR_MACROS := ESP_PLATFORM  MBEDTLS_CONFIG_FILE=\"mbedtls/esp_config.h\"  HAVE_CONFIG_H

ARDUINO_MACROS := F_CPU=240000000L \
					ARDUINO=10805 \
					ARDUINO_ESP32_DEV \
					ARDUINO_ARCH_ESP32 \
					ARDUINO_BOARD=\"ESP32_DEV\" \
					ESP32 CORE_DEBUG_LEVEL=0 \
					CONFIG_AUTOSTART_ARDUINO

PREPROCESSOR_MACROS += $(ARDUINO_MACROS)
