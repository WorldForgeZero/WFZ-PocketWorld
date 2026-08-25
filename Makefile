PYTHON = .venv/bin/python3

# Параметры Python
PY_CFLAGS  := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_config_var('CFLAGS') or '')")
PY_INCLUDE := $(shell $(PYTHON) -c "import sysconfig; print('-I' + sysconfig.get_path('include'))")
PYBIND11_INCLUDE := $(shell $(PYTHON) -m pybind11 --includes)
PY_LIBS := $(shell $(PYTHON) -c "import sysconfig; print('-L' + sysconfig.get_config_var('LIBDIR'), '-lpython' + sysconfig.get_config_var('VERSION'))")

# Компиляторы
CXX = g++
CC  = gcc

# Базовые флаги
SRC_DIR = wfz_pocketworld
LIB_DIR = lib
PYI_DIR = $(SRC_DIR)/python/pyi
DIST_DIR = dist

# Исключаемые файлы и каталоги
EXCLUDE_DIRS = \( -name '.*' -o -name '__*' \) -prune

# Исходники
CPP_SOURCES := $(shell find $(SRC_DIR) $(LIB_DIR) $(EXCLUDE_DIRS) -o -type f -name '*.cpp' -print)
C_SOURCES   := $(shell find $(SRC_DIR) $(LIB_DIR) $(EXCLUDE_DIRS) -o -type f -name '*.c' -print)

# Объектные файлы для режимов
DEBUG_CPP_OBJS   = $(patsubst %.cpp, build/debug/%.o, $(CPP_SOURCES))
DEBUG_C_OBJS     = $(patsubst %.c, build/debug/%.o, $(C_SOURCES))
DEBUG_OBJS       = $(DEBUG_CPP_OBJS) $(DEBUG_C_OBJS)

RELEASE_CPP_OBJS = $(patsubst %.cpp, build/release/%.o, $(CPP_SOURCES))
RELEASE_C_OBJS   = $(patsubst %.c, build/release/%.o, $(C_SOURCES))
RELEASE_OBJS     = $(RELEASE_CPP_OBJS) $(RELEASE_C_OBJS)

# Исполняемый файл
TARGET = server

# Пути включения
INCLUDE_DIRS := $(shell find $(SRC_DIR) $(LIB_DIR) $(EXCLUDE_DIRS) -o -type d -exec echo -I{} \;)

# Флаги C++ и C
COMMON_CXXFLAGS = -fPIC -Wall -Wextra $(PY_CFLAGS) $(PY_INCLUDE) $(PYBIND11_INCLUDE) $(INCLUDE_DIRS) -std=c++17
COMMON_CFLAGS   = -fPIC -Wall -Wextra $(PY_CFLAGS) $(PY_INCLUDE) $(INCLUDE_DIRS)

DEBUG_CXXFLAGS   = $(COMMON_CXXFLAGS) -g -O0
DEBUG_CFLAGS     = $(COMMON_CFLAGS) -g -O0

RELEASE_CXXFLAGS = $(COMMON_CXXFLAGS) -g -O3 -flto -fomit-frame-pointer -DNDEBUG
RELEASE_CFLAGS   = $(COMMON_CFLAGS) -g -O3 -flto -fomit-frame-pointer -DNDEBUG

# Линковочные флаги
BASE_LDFLAGS = -lpthread $(PY_LIBS)
DEBUG_LDFLAGS   = $(BASE_LDFLAGS)
RELEASE_LDFLAGS = $(BASE_LDFLAGS) -flto

# Цели
.PHONY: all debug release clean clear dist

all: debug
clear: clean

# Компиляция C++ в объектные файлы (debug)
build/debug/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(DEBUG_CXXFLAGS) -MMD -c $< -o $@

# Компиляция C в объектные файлы (debug)
build/debug/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(DEBUG_CFLAGS) -MMD -c $< -o $@

# Компиляция C++ в объектные файлы (release)
build/release/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(RELEASE_CXXFLAGS) -MMD -c $< -o $@

# Компиляция C в объектные файлы (release)
build/release/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(RELEASE_CFLAGS) -MMD -c $< -o $@

# Линковка debug
debug: $(DEBUG_OBJS)
	$(CXX) $(DEBUG_OBJS) $(DEBUG_LDFLAGS) -o $(TARGET)

# Линковка release
release: $(RELEASE_OBJS)
	$(CXX) $(RELEASE_OBJS) $(RELEASE_LDFLAGS) -o $(TARGET)

# Создание папки распространения
dist: release
	@rm -rf $(DIST_DIR)
	@mkdir -p $(DIST_DIR)
	@mkdir -p $(DIST_DIR)/wfz
	cp $(TARGET) $(DIST_DIR)/
	@rm -f $(TARGET)
	cp $(PYI_DIR)/*.pyi $(DIST_DIR)/wfz/ 2>/dev/null || true
	cp $(PYI_DIR)/*.py $(DIST_DIR)/wfz/ 2>/dev/null || true
	@echo "Дистрибутив собран в $(DIST_DIR)/"

# Зависимости
-include build/debug/*.d build/release/*.d

# Очистка
clean:
	rm -rf build/ $(TARGET) $(DIST_DIR)
	find $(SRC_DIR) $(LIB_DIR) -type f \( -name '*.o' -o -name '*.d' \) -delete
