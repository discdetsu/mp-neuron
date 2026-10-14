CFLAGS   ?= -std=c11 -Wall -Wextra -Wpedantic -g
CPPFLAGS += -Iinclude

BUILD  := build
TARGET := $(BUILD)/mp_neuron
SRCS   := src/main.c src/mp_neuron.c
OBJS   := $(SRCS:src/%.c=$(BUILD)/%.o)
DEPS   := $(OBJS:.o=.d)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BUILD)/%.o: src/%.c | $(BUILD)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c -o $@ $<

$(BUILD):
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BUILD)

-include $(DEPS)
