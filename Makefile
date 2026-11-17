CC ?= gcc
CXX ?= g++

SRC_DIR := src
INCLUDE_DIR := include
BUILD_DIR := build
TEST_O_DIR := $(BUILD_DIR)/test

DEPS_DIR := third_party
MAGIC_ENUM_DIR := $(DEPS_DIR)/magic_enum

TARGET := $(BUILD_DIR)/ccshell

WARNFLAGS := -Wall -Wextra

ALL_CPPFLAGS := -I$(INCLUDE_DIR) -I$(MAGIC_ENUM_DIR)/include -MMD -MP $(CPPFLAGS)
ALL_CXXFLAGS := -std=c++23 $(WARNFLAGS) $(OPTFLAGS) $(LTOFLAGS) $(CXXFLAGS)
ALL_LDFLAGS  := $(OPTFLAGS) $(LTOFLAGS)

TARGET_OS ?= $(shell uname -s)

ALL_LDFLAGS += $(LDFLAGS) $(EXTRA_LDFLAGS)

SRCS := $(shell find $(SRC_DIR) -name '*.cc')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

TEST_SRCS := $(shell find test -name 'driver.cc')
TEST_BINS := $(TEST_SRCS:%.cc=$(BUILD_DIR)/%.out)
DEPS += $(TEST_SRCS:%=$(BUILD_DIR)/%.d)
LIB_OBJS := $(filter-out %/main.cc.o,$(OBJS))

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
    OPTFLAGS := -O3 -DNDEBUG
else
    # debug
    LTO := 0
    OPTFLAGS := -O0
    OPTFLAGS += -ggdb3 -fno-omit-frame-pointer -fno-inline -D_GLIBCXX_ASSERTIONS
endif

ifeq ($(LTO),1)
    LTOFLAGS := -flto=auto
endif


ifeq ($(STATIC),1)
    ifeq ($(TARGET_OS),Linux)
        LDFLAGS += -static -static-libgcc -static-libstdc++
    else ifeq ($(TARGET_OS),Darwin)
        # empty
    else
        # empty
    endif
endif


$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(ALL_LDFLAGS) $^ $(LDLIBS) -o $@

$(BUILD_DIR)/%.cc.o: %.cc
	@mkdir -p $(dir $@)
	$(CXX) $(ALL_CPPFLAGS) $(ALL_CXXFLAGS) -c $< -o $@

-include $(DEPS)

.PHONY: all clean fresh magic-enum test

magic-enum:
	@mkdir -p $(DEPS_DIR)
	@if [ -d "$(MAGIC_ENUM_DIR)/.git" ]; then \
		git -C "$(MAGIC_ENUM_DIR)" pull --ff-only; \
	else \
		git clone --depth 1 https://github.com/Neargye/magic_enum.git "$(MAGIC_ENUM_DIR)" -b master --single-branch; \
	fi

clean:
	rm -rf $(BUILD_DIR)

fresh:
	$(MAKE) clean
	$(MAKE) DEBUG=$(DEBUG) STATIC=$(STATIC)

all: $(TARGET)

# test

$(BUILD_DIR)/test/%/driver.out: $(BUILD_DIR)/test/%/driver.cc.o $(LIB_OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(ALL_LDFLAGS) $^ $(LDLIBS) -o $@

test: $(TEST_BINS)
