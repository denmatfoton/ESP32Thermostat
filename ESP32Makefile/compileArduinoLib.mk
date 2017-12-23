#build Arduino library
include common.mk
include esp32config.mk


$(info _____________________Looking for library $(LIB_NAME)________________________)
ARDUINO_LIB_PATH := $(strip $(call find_arduino_lib,$(LIB_NAME)))
ifeq ("$(ARDUINO_LIB_PATH)"," ")
	$(error Error!!! Library $(LIB_NAME) not found)
endif
$(info Library path $(ARDUINO_LIB_PATH))
$(info Start library $(LIB_NAME) compilation)


ARDUINO_LIB_INCLUDE_DIRS := $(ESP32_INCLUDE_DIRS)
ARDUINO_LIB_INCLUDE_DIRS += $(ADDITIONAL_INCLUDES)


ARDUINO_LIB_OBJDIR := $(OBJDIR)/$(LIB_NAME)
ARDUINO_LIB_TARGET := $(call get_lib_target,$(LIB_NAME))


# Make does not offer a recursive wildcard function, so here's one:
rwildcard=$(wildcard $1$2)$(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))


ARDUINO_LIB_SRC := $(wildcard $(ARDUINO_LIB_PATH)/*.c)
ARDUINO_LIB_SRC += $(strip $(call rwildcard,$(ARDUINO_LIB_PATH)/src/,*.c))

ARDUINO_LIB_SRCXX := $(wildcard $(ARDUINO_LIB_PATH)/*.cpp)
ARDUINO_LIB_SRCXX += $(strip $(call rwildcard,$(ARDUINO_LIB_PATH)/src/,*.cpp))

ARDUINO_LIB_SOURCEFILES := $(ARDUINO_LIB_SRC) $(ARDUINO_LIB_SRCXX)


ARDUINO_LIB_SOURCE_DIRS := $(dir $(ARDUINO_LIB_SOURCEFILES))
VPATH = $(sort $(ARDUINO_LIB_SOURCE_DIRS))


ARDUINO_LIB_CXXFLAGS := $(ESP32_CXXFLAGS)
ARDUINO_LIB_CFLAGS := $(ESP32_CFLAGS)

ARDUINO_LIB_CXXFLAGS += $(addprefix -I,$(ARDUINO_LIB_INCLUDE_DIRS))
ARDUINO_LIB_CFLAGS += $(addprefix -I,$(ARDUINO_LIB_INCLUDE_DIRS))

ARDUINO_LIB_CXXFLAGS += $(addprefix -D,$(PREPROCESSOR_MACROS))
ARDUINO_LIB_CFLAGS += $(addprefix -D,$(PREPROCESSOR_MACROS))


source_obj1 := $(ARDUINO_LIB_SOURCEFILES:.cpp=.o)
source_objs := $(source_obj1:.c=.o)

all_objs := $(addprefix $(ARDUINO_LIB_OBJDIR)/, $(notdir $(source_objs)))



all: create_dirs $(ARDUINO_LIB_TARGET)

	
$(ARDUINO_LIB_TARGET): $(all_objs)
	$(AR) cru $@ $^
	
create_dirs:
	mkdir -p $(ARDUINO_LIB_OBJDIR)


$(ARDUINO_LIB_OBJDIR)/%.o : %.cpp
	@echo
	@echo $(MSG_COMPILING_CPP) $<
	$(CXX) $(ARDUINO_LIB_CXXFLAGS) -c $< -o $@

$(ARDUINO_LIB_OBJDIR)/%.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) $(ARDUINO_LIB_CFLAGS) -c $< -o $@
