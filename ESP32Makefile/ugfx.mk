#build Arduino library
include common.mk
include esp32config.mk

GFX_OBJDIR := $(OBJDIR)/gfx
GFX_TARGET := $(strip $(call get_lib_target,gfx))

GFXLIB := ugfx28

OPT_OS := freertos
#OPT_OS := arduino

DISPLAY_PATH := $(GFXLIB)/drivers/gdisp/ILI9341
TOUCH_PATH := $(GFXLIB)/drivers/ginput/touch/ADS7843


GFXSRC := $(wildcard $(DISPLAY_PATH)/*.c)
GFXSRC += $(wildcard $(DISPLAY_PATH)/*.cpp)
GFXSRC += $(wildcard $(TOUCH_PATH)/*.c)
GFXSRC += $(wildcard $(TOUCH_PATH)/*.cpp)

GFXINC := $(DISPLAY_PATH) $(TOUCH_PATH)

include $(GFXLIB)/gfx.mk
$(file >gfx.inc,$(GFXINC))


GFXINC += $(ESP32_INCLUDE_DIRS)
GFXINC += $(ADDITIONAL_INCLUDES)


GFX_SOURCE_DIRS := $(dir $(GFXSRC))
VPATH = $(sort $(GFX_SOURCE_DIRS))


GFX_CFLAGS := $(ESP32_CFLAGS)
GFX_CFLAGS += $(addprefix -I,$(GFXINC))
GFX_CFLAGS += $(addprefix -D,$(PREPROCESSOR_MACROS))


GFX_CXXFLAGS := $(ESP32_CXXFLAGS)
GFX_CXXFLAGS += $(addprefix -I,$(GFXINC))
GFX_CXXFLAGS += $(addprefix -D,$(PREPROCESSOR_MACROS))


source_obj1 := $(GFXSRC:.cpp=.o)
source_objs := $(source_obj1:.c=.o)

all_objs := $(addprefix $(GFX_OBJDIR)/, $(notdir $(source_objs)))


all: create_dirs $(GFX_TARGET)

	
$(GFX_TARGET): $(all_objs)
	$(AR) cru $@ $^
	
create_dirs:
	mkdir -p $(GFX_OBJDIR)

$(GFX_OBJDIR)/%.o : %.cpp
	@echo
	@echo $(MSG_COMPILING_CPP) $<
	$(CXX) $(GFX_CXXFLAGS) -c $< -o $@

$(GFX_OBJDIR)/%.o : %.c
	@echo
	@echo $(MSG_COMPILING) $<
	$(CC) $(GFX_CFLAGS) -c $< -o $@
