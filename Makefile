# Project
TARGET := server

SRC_DIR := src
THIRD_PARTY_DIR := third_party

BUILD_DIR := build
DIST_DIR := dist

# Compiler
CXX := g++

# Source discovery
FIND_EXCLUDES := \( -name '.*' -o -name '__*' -o -name 'old' \) -prune -o

SOURCES := $(shell \
	find $(SRC_DIR) \
	$(FIND_EXCLUDES) \
	-type f -name '*.cpp' -print)

INCLUDE_ROOTS := $(SRC_DIR) $(wildcard $(THIRD_PARTY_DIR))

INCLUDE_DIRS := $(shell \
	find $(INCLUDE_ROOTS) \
	$(FIND_EXCLUDES) \
	-type d -print)

INCLUDE_FLAGS := $(addprefix -I,$(INCLUDE_DIRS))

# Objects
DEBUG_OBJS := $(patsubst %.cpp,$(BUILD_DIR)/debug/%.o,$(SOURCES))
RELEASE_OBJS := $(patsubst %.cpp,$(BUILD_DIR)/release/%.o,$(SOURCES))

DEPS := \
	$(DEBUG_OBJS:.o=.d) \
	$(RELEASE_OBJS:.o=.d)

# Compiler flags
COMMON_CXXFLAGS := \
	-std=c++17 \
	-Wall \
	-Wextra \
	-pthread \
	$(INCLUDE_FLAGS)

DEBUG_CXXFLAGS := \
	$(COMMON_CXXFLAGS) \
	-O0 \
	-g

NATIVE_FLAGS ?=

RELEASE_CXXFLAGS := \
	$(COMMON_CXXFLAGS) \
	-O3 \
	-flto \
	-DNDEBUG \
	$(NATIVE_FLAGS)

# Linker flags
DEBUG_LDFLAGS := -pthread
RELEASE_LDFLAGS := -pthread -flto

LDLIBS ?=

# Output binaries
DEBUG_BIN := $(BUILD_DIR)/debug/$(TARGET)
RELEASE_BIN := $(BUILD_DIR)/release/$(TARGET)

# Targets
.PHONY: all debug release dist dist-release clean clear

all: debug

debug: $(DEBUG_BIN)

release: $(RELEASE_BIN)

# Debug objects
$(BUILD_DIR)/debug/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(DEBUG_CXXFLAGS) -MMD -MP -c $< -o $@

# Release objects
$(BUILD_DIR)/release/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(RELEASE_CXXFLAGS) -MMD -MP -c $< -o $@

# Debug linking
$(DEBUG_BIN): $(DEBUG_OBJS)
	@mkdir -p $(@D)
	$(CXX) $^ $(DEBUG_LDFLAGS) $(LDLIBS) -o $@

# Release linking
$(RELEASE_BIN): $(RELEASE_OBJS)
	@mkdir -p $(@D)
	$(CXX) $^ $(RELEASE_LDFLAGS) $(LDLIBS) -o $@

# Distribution
DIST_MODE ?= debug

dist: $(BUILD_DIR)/$(DIST_MODE)/$(TARGET)
	@mkdir -p $(DIST_DIR)
	cp $< $(DIST_DIR)/$(TARGET)
	@echo "Distribution ready: $(DIST_DIR)/$(TARGET) ($(DIST_MODE))"

dist-release:
	$(MAKE) dist DIST_MODE=release

# Dependencies
-include $(DEPS)

# Cleanup
clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)

clear: clean
