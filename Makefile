CXX = g++
SRC_DIR = src
INCLUDE_DIR = include
BUILD = build
TARGET = $(BUILD)/ccshell

CPPFLAGS = -I$(INCLUDE_DIR) -MMD -MP
CXXFLAGS = -std=c++23 -Wall -Wextra -O2

# static
LDFLAGS += -static -static-libgcc -static-libstdc++

SRCS = $(shell find $(SRC_DIR) -name '*.cc')
OBJS = $(SRCS:%=$(BUILD)/%.o)
DEPS = $(OBJS:.o=.d)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD)/%.cc.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -rf $(BUILD)

.PHONY: clean
