TARGET_EXEC ?= fft_c

BUILD_DIR ?= ./build
SRC_DIRS ?= ./src
RESOURCE_DIR ?= ./resources

CC = gcc
ASM = nasm

SRCS := $(shell find $(SRC_DIRS) -name *.c -or -name *.asm)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS ?= $(INC_FLAGS) -MMD -MP
CFLAGS ?= -Wall
LDFLAGS ?= -lfftw3 -lm
ASFLAGS ?= -f elf64

$(BUILD_DIR)/$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# assembly
$(BUILD_DIR)/%.asm.o: %.asm
	$(MKDIR_P) $(dir $@)
	$(ASM) $(ASFLAGS) $< -o $@

# c source
$(BUILD_DIR)/%.c.o: %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@


.PHONY: run memcheck clean
run:
	@$(BUILD_DIR)/$(TARGET_EXEC) $(RESOURCE_DIR)/Transmit_1.wav \
		$(RESOURCE_DIR)/Javelin_receive_1.wav $(RESOURCE_DIR)/Shotput_receive_1.wav

# .PHONY: memcheck
memcheck:
	valgrind --leak-check=full --show-leak-kinds=all $(BUILD_DIR)/$(TARGET_EXEC) \
		$(RESOURCE_DIR)/Transmit_1.wav $(RESOURCE_DIR)/Javelin_receive_1.wav \
		$(RESOURCE_DIR)/Shotput_receive_1.wav

# .PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR)/*

-include $(DEPS)

MKDIR_P ?= mkdir -p