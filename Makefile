PYTHON ?= .venv/bin/python3

MAKEFLAGS += -j$(shell nproc 2>/dev/null || echo 4)

# Project
TARGET          := server
SRC_DIR         := wfz_pocketworld
THIRD_PARTY_DIR := third_party
BUILD_DIR       := build
DIST_DIR        := dist

PYI_DIR := $(SRC_DIR)/python/pyi

# Compiler
CXX := g++

# Python / pybind11
PYBIND11_INCLUDES := $(shell $(PYTHON) -m pybind11 --includes)

PY_LIBDIR := $(shell $(PYTHON) -c \
	"import sysconfig; print(sysconfig.get_config_var('LIBDIR') or '')")

PY_LDVERSION := $(shell $(PYTHON) -c \
	"import sysconfig; print(sysconfig.get_config_var('LDVERSION') or sysconfig.get_config_var('VERSION') or '')")

PY_LIBS := $(shell $(PYTHON) -c \
	"import sysconfig; print(sysconfig.get_config_var('LIBS') or '')")

PY_SYSLIBS := $(shell $(PYTHON) -c \
	"import sysconfig; print(sysconfig.get_config_var('SYSLIBS') or '')")

PY_LINKFORSHARED := $(shell $(PYTHON) -c \
	"import sysconfig; print(sysconfig.get_config_var('LINKFORSHARED') or '')")

PY_EMBED_LDFLAGS := \
	-L$(PY_LIBDIR) \
	-lpython$(PY_LDVERSION) \
	$(PY_LIBS) \
	$(PY_SYSLIBS) \
	$(PY_LINKFORSHARED)

# Source discovery
FIND_EXCLUDES := \( -name '.*' -o -name '__*' \) -prune -o

CPP_SOURCES := $(shell \
	find $(SRC_DIR) \
	$(FIND_EXCLUDES) \
	-type f -name '*.cpp' -print)

# Include paths
INCLUDE_DIRS := $(shell \
	find $(SRC_DIR) $(THIRD_PARTY_DIR) \
	$(FIND_EXCLUDES) \
	-type d -print | sed 's|^|-I|')

# Objects
DEBUG_OBJS := $(patsubst %.cpp,$(BUILD_DIR)/debug/%.o,$(CPP_SOURCES))

RELEASE_OBJS := $(patsubst %.cpp,$(BUILD_DIR)/release/%.o,$(CPP_SOURCES))

DEBUG_DEPS   := $(DEBUG_OBJS:.o=.d)
RELEASE_DEPS := $(RELEASE_OBJS:.o=.d)

# Compiler flags
COMMON_CXXFLAGS := \
	-fPIC \
	-pthread \
	-Wall \
	-Wextra \
	$(PYBIND11_INCLUDES) \
	$(INCLUDE_DIRS) \
	-std=c++17

DEBUG_CXXFLAGS := \
	$(COMMON_CXXFLAGS) \
	-g \
	-O0

NATIVE_FLAGS ?= -march=native -mtune=native

RELEASE_OPT_FLAGS ?= \
	-O3 \
	-flto \
	$(NATIVE_FLAGS) \
	-fomit-frame-pointer

RELEASE_CXXFLAGS := \
	$(COMMON_CXXFLAGS) \
	$(RELEASE_OPT_FLAGS) \
	-DNDEBUG

# Linker flags
BASE_LDFLAGS := \
	-pthread \
	$(PY_EMBED_LDFLAGS)

DEBUG_LDFLAGS := \
	$(BASE_LDFLAGS)

RELEASE_LDFLAGS := \
	$(BASE_LDFLAGS) \
	-flto

# Targets
.PHONY: \
	all \
	debug \
	release \
	dist \
	dist-release \
	clean \
	clear \
	python-info

all: debug

clear: clean

# Debug objects
$(BUILD_DIR)/debug/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(DEBUG_CXXFLAGS) -MMD -MP -c $< -o $@

# Release objects
$(BUILD_DIR)/release/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(RELEASE_CXXFLAGS) -MMD -MP -c $< -o $@

# Linking
debug: $(DEBUG_OBJS)
	$(CXX) $(DEBUG_OBJS) $(DEBUG_LDFLAGS) -o $(TARGET)

release: $(RELEASE_OBJS)
	$(CXX) $(RELEASE_OBJS) $(RELEASE_LDFLAGS) -o $(TARGET)

# Distribution
DIST_MODE ?= debug

dist: $(DIST_MODE)
	@rm -rf $(DIST_DIR)
	@mkdir -p $(DIST_DIR)/wfz
	@mkdir -p $(DIST_DIR)/python

	cp $(TARGET) $(DIST_DIR)/

	@if [ -d "$(PYI_DIR)" ]; then \
		cp -a "$(PYI_DIR)/." "$(DIST_DIR)/wfz/"; \
	fi

	@touch $(DIST_DIR)/python/autorun.py

	@echo "Дистрибутив собран в $(DIST_DIR)/ (режим: $(DIST_MODE))"

dist-release:
	$(MAKE) dist DIST_MODE=release

# Python diagnostics
python-info:
	@$(PYTHON) -c \
		"import sys, sysconfig; \
		print('Executable:       ', sys.executable); \
		print('Version:          ', sys.version.replace('\n', ' ')); \
		print('ABI flags:        ', sys.abiflags); \
		print('LDVERSION:        ', sysconfig.get_config_var('LDVERSION')); \
		print('Py_GIL_DISABLED:  ', sysconfig.get_config_var('Py_GIL_DISABLED')); \
		print('GIL enabled:      ', sys._is_gil_enabled() if hasattr(sys, '_is_gil_enabled') else 'unknown'); \
		print('LIBDIR:           ', sysconfig.get_config_var('LIBDIR'))"

# Dependency files
-include $(DEBUG_DEPS)
-include $(RELEASE_DEPS)

# Cleanup
clean:
	rm -f $(TARGET)
	rm -rf $(DIST_DIR)
	rm -rf $(BUILD_DIR)
