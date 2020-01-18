# Makefile for the STM32F030 skeletion
#
# John Berg @ netbasenext.nl
#

PROJECT = firmware

# Project Structure
SRCDIR = src
BINDIR = bin
OBJDIR = obj
INCDIR = include
COMDIR = common

# Project target
CPU = cortex-m0

# Sources
SRC = $(wildcard $(SRCDIR)/*.c) $(wildcard $(COMDIR)/src/*.c) 
CPPSRC = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(COMDIR)/src/*.cpp)
ASM = $(wildcard $(SRCDIR)/*.s) $(wildcard $(COMDIR)/src/*.s)

#defines
DEFINE = -DSTM32F030x6

# Include directories
INCLUDE  = -I$(INCDIR) -I$(COMDIR)/include -I$(COMDIR)/cmsis

# Linker
LSCRIPT = STM32F030F4Px.ld

# C/C++ Flags
CCOMMONFLAGS = -Os -g -mcpu=$(CPU) -mthumb  -Wall  -fno-common  --specs=nosys.specs --specs=nano.specs

# Generate dependency information
CDEPFLAGS += -MMD -MP -MF $(@:%.o=%.d)

# C Flags
GCFLAGS  = -std=c++17 -fverbose-asm
GCFLAGS += $(CCOMMONFLAGS) $(INCLUDE) $(DEFINE) $(CDEPFLAGS)
LDFLAGS += -T$(LSCRIPT) -mthumb -mcpu=$(CPU) --specs=nosys.specs --specs=nano.specs -Wl,-Map,$(BINDIR)/$(PROJECT).map -Wl,--gc-sections
ASFLAGS += -mcpu=$(CPU)

# CPP Flags
CPPFLAGS = -fno-exceptions -fno-rtti

# Tools
CC = arm-none-eabi-gcc
CP = arm-none-eabi-g++
AS = arm-none-eabi-as
AR = arm-none-eabi-ar
LD = arm-none-eabi-ld
OBJCOPY = arm-none-eabi-objcopy
SIZE = arm-none-eabi-size
OBJDUMP = arm-none-eabi-objdump

RM = rm -rf

## Build process
OBJ := $(addprefix $(OBJDIR)/, $(notdir $(SRC:.c=.o)))
OBJ += $(addprefix $(OBJDIR)/, $(notdir $(ASM:.s=.o)))
CPPOBJ := $(addprefix $(OBJDIR)/, $(notdir $(CPPSRC:.cpp=.o)))


all:: $(BINDIR)/$(PROJECT).bin $(BINDIR)/$(PROJECT).hex

Build: $(BINDIR)/$(PROJECT).bin

macros:
	$(CP) $(GCFLAGS) -dM -E - < /dev/null

cleanBuild: clean

clean:
	$(RM) $(BINDIR)
	$(RM) $(OBJDIR)

size:
	@$(SIZE) -A -x $(BINDIR)/$(PROJECT).elf
	@$(SIZE) -A $(BINDIR)/$(PROJECT).elf

makedebug:
	@echo $(OBJ)
	@echo $(CPPOBJ)

#convert to hex
$(BINDIR)/$(PROJECT).hex: $(BINDIR)/$(PROJECT).elf
	$(OBJCOPY) -O ihex $(BINDIR)/$(PROJECT).elf $(BINDIR)/$(PROJECT).hex
	@echo -e ""
	@$(SIZE) -A -x $(BINDIR)/$(PROJECT).elf

#convert to bin
$(BINDIR)/$(PROJECT).bin: $(BINDIR)/$(PROJECT).elf
	$(OBJCOPY) -O binary $(BINDIR)/$(PROJECT).elf $(BINDIR)/$(PROJECT).bin

# linking
$(BINDIR)/$(PROJECT).elf: $(OBJ) $(CPPOBJ) $(LSCRIPT)
	@mkdir -p $(dir $@)
	$(CP) $(OBJ) $(CPPOBJ) $(LDFLAGS) -o $(BINDIR)/$(PROJECT).elf
	$(OBJDUMP) -drwCS -marm $(BINDIR)/$(PROJECT).elf > $(BINDIR)/$(PROJECT).S

# Compilation
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CP) $(GCFLAGS) $(CPPFLAGS) -c $< -o $@
	$(OBJDUMP) -drwCS -marm $@ > $(OBJDIR)/$(notdir $<).S
	@echo -e ""

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CP) $(GCFLAGS) $(CPPFLAGS) -c $< -o $@
	$(OBJDUMP) -drwCS -marm $@ > $(OBJDIR)/$(notdir $<).S
	@echo -e ""

$(OBJDIR)/%.o: $(SRCDIR)/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<
	@echo -e ""

$(OBJDIR)/%.o: $(COMDIR)/src/%.c
	@mkdir -p $(dir $@)
	$(CP) $(GCFLAGS) $(CPPFLAGS) -c $< -o $@
	@echo -e ""

$(OBJDIR)/%.o: $(COMDIR)/src/%.cpp
	@mkdir -p $(dir $@)
	$(CP) $(GCFLAGS) $(CPPFLAGS) -c $< -o $@
	@echo -e ""

$(OBJDIR)/%.o: $(COMDIR)/src/%.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<

$(OBJ): Makefile

-include $(wildcard $(OBJDIR)/*.d)
