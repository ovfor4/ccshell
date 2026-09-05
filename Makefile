CC = gcc
CXX = g++
SRC_DIR = src
INCLUDE_DIR = include
BUILD = build
TARGET = $(BUILD)/ccshell

DEPS_DIR = third_party
MAGIC_ENUM_DIR = $(DEPS_DIR)/magic_enum

MAKEFLAGS += -j8

CPPFLAGS = -I$(INCLUDE_DIR) -MMD -MP -flto
CXXFLAGS = -std=c++23 -Wall -Wextra -O3 -flto
CPPFLAGS += -I$(MAGIC_ENUM_DIR)/include


TARGET_OS ?= $(shell uname -s)

ifeq ($(TARGET_OS),Linux)
    LDFLAGS += -static -static-libgcc -static-libstdc++
else ifeq ($(TARGET_OS),Darwin)
    # empty
else
    # empty
endif

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

fresh:
	rm -rf $(BUILD)
	make

.PHONY: clean

.PHONY: magic-enum

magic-enum:
	@mkdir -p $(DEPS_DIR)
	@if [ -d "$(MAGIC_ENUM_DIR)/.git" ]; then \
		git -C "$(MAGIC_ENUM_DIR)" pull --ff-only; \
	else \
		git clone --depth 1 https://github.com/Neargye/magic_enum.git "$(MAGIC_ENUM_DIR)" -b master --single-branch; \
	fi
