CC = gcc
CXX ?= g++

SRC_DIR := src
INCLUDE_DIR := include
BUILD_ROOT := build

DEPS_DIR := third_party
MAGIC_ENUM_DIR := $(DEPS_DIR)/magic_enum

ifeq ($(MAKELEVEL),0)
	MAKEFLAGS += -j8
endif

#   make                # release：-O3 + LTO
#   make DEBUG=1        # debug: -Og -g3 and no LTO
#   make STATIC=1       # static (maybe tricky)
DEBUG  ?= 0
LTO    ?= 1
STATIC ?= 0

ifeq ($(DEBUG),0)
    # release
    CONFIG := release
    OPTFLAGS := -O3 -DNDEBUG
else
    # debug
    LTO := 0
    CONFIG   := debug
    OPTFLAGS := -Og
    OPTFLAGS += -g3 -fno-omit-frame-pointer -D_GLIBCXX_ASSERTIONS
endif

ifeq ($(LTO),1)
    LTOFLAGS := -flto=auto
endif

BUILD  := $(BUILD_ROOT)/$(CONFIG)
TARGET := $(BUILD)/ccshell

WARNFLAGS := -Wall -Wextra

ALL_CPPFLAGS := -I$(INCLUDE_DIR) -I$(MAGIC_ENUM_DIR)/include -MMD -MP $(CPPFLAGS)
ALL_CXXFLAGS := -std=c++23 $(WARNFLAGS) $(OPTFLAGS) $(LTOFLAGS) $(CXXFLAGS)
ALL_LDFLAGS  := $(OPTFLAGS) $(LTOFLAGS)

TARGET_OS ?= $(shell uname -s)

ifeq ($(STATIC),1)
    ifeq ($(TARGET_OS),Linux)
        LDFLAGS += -static -static-libgcc -static-libstdc++
    else ifeq ($(TARGET_OS),Darwin)
        # empty
    else
        # empty
    endif
endif

ALL_LDFLAGS += $(LDFLAGS) $(EXTRA_LDFLAGS)

SRCS := $(shell find $(SRC_DIR) -name '*.cc')
OBJS := $(SRCS:%=$(BUILD)/%.o)
DEPS := $(OBJS:.o=.d)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(ALL_LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD)/%.cc.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -c $< -o $@

-include $(DEPS)

.PHONY: clean fresh magic-enum 

magic-enum:
	@mkdir -p $(DEPS_DIR)
	@if [ -d "$(MAGIC_ENUM_DIR)/.git" ]; then \
		git -C "$(MAGIC_ENUM_DIR)" pull --ff-only; \
	else \
		git clone --depth 1 https://github.com/Neargye/magic_enum.git "$(MAGIC_ENUM_DIR)" -b master --single-branch; \
	fi

clean:
	rm -rf $(BUILD_ROOT)

fresh:
	$(MAKE) clean
	$(MAKE)

