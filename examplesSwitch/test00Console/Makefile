#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

#---------------------------------------------------------------------------------
TARGET      :=  $(notdir $(CURDIR))
BUILD       :=  build
SOURCES     :=  .
DATA        :=  data
INCLUDES    :=  .

#---------------------------------------------------------------------------------
ARCH    :=  -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE

LIBS    := -lglad -lEGL -lglapi -ldrm_nouveau -lnx

CFLAGS  :=  -g -Wall -O2 -ffunction-sections $(ARCH) -D__SWITCH__
CFLAGS  +=  $(INCLUDE)

CXXFLAGS    := $(CFLAGS) -fno-rtti -fno-exceptions

ASFLAGS :=  -g $(ARCH)
LDFLAGS =   -specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)

#---------------------------------------------------------------------------------
LIBDIRS := $(PORTLIBS) $(LIBNX)

#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT   :=  $(CURDIR)/$(TARGET)
export TOPDIR   :=  $(CURDIR)

export VPATH    :=  $(foreach dir,$(SOURCES),$(TOPDIR)/$(dir)) \
                    $(foreach dir,$(DATA),$(TOPDIR)/$(dir))

export DEPSDIR  :=  $(CURDIR)/$(BUILD)

CFILES      :=  $(wildcard *.c)
CPPFILES    :=  $(wildcard *.cpp)
SFILES      :=  $(wildcard *.s)
BINFILES    :=  $(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

ifeq ($(strip $(CPPFILES)),)
    export LD   :=  $(CC)
else
    export LD   :=  $(CXX)
endif

export OFILES_BIN   :=  $(addsuffix .o,$(BINFILES))
export OFILES_SRC   :=  $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES       :=  $(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN   :=  $(addsuffix .h,$(subst .,_,$(BINFILES)))

export INCLUDE  :=  $(foreach dir,$(INCLUDES),-I$(TOPDIR)/$(dir)) \
                    $(foreach dir,$(LIBDIRS),-I$(dir)/include) \
                    -I$(CURDIR)/$(BUILD)

export LIBPATHS :=  $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

.PHONY: $(BUILD) clean all

all: $(BUILD)

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo clean ...
	@rm -fr $(BUILD) $(TARGET).nro $(TARGET).nacp $(TARGET).elf

#---------------------------------------------------------------------------------
else
.PHONY: all

DEPENDS :=  $(OFILES:.o=.d)

# Esta es la parte que hemos modificado:
all : $(OUTPUT).nro
	@rm -f $(OUTPUT).elf $(OUTPUT).nacp
	@echo "Build finished: $(notdir $(OUTPUT)).nro"

$(OUTPUT).nro : $(OUTPUT).elf $(OUTPUT).nacp

$(OUTPUT).elf : $(OFILES)

$(OUTPUT).nacp : $(OFILES)

$(OFILES_SRC) : $(HFILES_BIN)

%.bin.o %_bin.h : %.bin
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

endif