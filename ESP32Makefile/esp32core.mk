#ESP32 core build
include common.mk
include esp32config.mk

ESP32_SRC := $(wildcard $(ESP32_CORE_PATH)/*.c)
ESP32_SRC += $(wildcard $(ESP32_CORE_PATH)/libb64/*.c)


ESP32_SRCXX := $(wildcard $(ESP32_CORE_PATH)/*.cpp)


ESP32_CFLAGS += $(addprefix -I,$(ESP32_INCLUDE_DIRS))
ESP32_CXXFLAGS += $(addprefix -I,$(ESP32_INCLUDE_DIRS))

ESP32_CFLAGS += $(addprefix -D,$(PREPROCESSOR_MACROS))
ESP32_CFLAGS += $(addprefix -D,$(PREPROCESSOR_MACROS))


ESP32_SOURCEFILES := $(ESP32_SRC) $(ESP32_SRCXX)


ESP32_SOURCE_DIRS := $(dir $(ESP32_SOURCEFILES))
VPATH = $(sort $(ESP32_SOURCE_DIRS))

source_obj1 := $(ESP32_SOURCEFILES:.cpp=.o)
source_objs := $(source_obj1:.c=.o)

all_objs := $(addprefix $(ESP32_OBJDIR)/, $(notdir $(source_objs)))


all: create_dirs $(ESP32_CORE_TARGET)

	
$(ESP32_CORE_TARGET): $(all_objs)
	$(AR) cru $@ $^
	
create_dirs:
	mkdir -p $(ESP32_OBJDIR)


 $(ESP32_OBJDIR)/%.o : %.cpp
	@echo
	@echo $(MSG_COMPILING_CPP) $<
	$(CXX) $(ESP32_CXXFLAGS) -c $< -o $@

$(ESP32_OBJDIR)/%.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) $(ESP32_CFLAGS) -c $< -o $@

