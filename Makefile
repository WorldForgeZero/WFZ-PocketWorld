PYTHON = .venv/bin/python3

# Параметры Python
PY_CFLAGS  := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_config_var('CFLAGS') or '')")
PY_INCLUDE := $(shell $(PYTHON) -c "import sysconfig; print('-I' + sysconfig.get_path('include'))")
PYBIND11_INCLUDE := $(shell $(PYTHON) -m pybind11 --includes)

# Компилятор
CXX = g++
CXXFLAGS = -std=c++17

# Общие флаги
INCLUDE_DIRS := $(shell find $(SRC_DIR) -type d -not -path '*/.venv/*' -not -path '*/.git/*' -not -path '*/__pycache__/*' -exec echo -I{} \;)
BASE_CFLAGS = -fPIC -Wall -Wextra $(PY_CFLAGS) $(PY_INCLUDE) $(INCLUDE_DIRS) $(PYBIND11_INCLUDE) $(CXXFLAGS)
BASE_LDFLAGS = -shared -lpthread

# Режимы
DEBUG_CFLAGS   = -g -O0
RELEASE_CFLAGS = -O3 -flto -fomit-frame-pointer -DNDEBUG
RELEASE_LDFLAGS = -flto

# Исходники
SRC_DIR = wfz_pocketworld
SOURCES = $(shell find $(SRC_DIR) -name '*.cpp')
TARGET  = $(SRC_DIR)/_core.so

# Объектные файлы для разных режимов
DEBUG_OBJS   = $(patsubst $(SRC_DIR)/%.cpp, build/debug/%.o, $(SOURCES))
RELEASE_OBJS = $(patsubst $(SRC_DIR)/%.cpp, build/release/%.o, $(SOURCES))

# Цели
.PHONY: all debug release clean clear

all: debug
clear: clean

debug: CFLAGS  = $(BASE_CFLAGS) $(DEBUG_CFLAGS)
debug: LDFLAGS = $(BASE_LDFLAGS)
debug: $(TARGET)

release: CFLAGS  = $(BASE_CFLAGS) $(RELEASE_CFLAGS)
release: LDFLAGS = $(BASE_LDFLAGS) $(RELEASE_LDFLAGS)
release: $(TARGET)

# Сборка .so
$(TARGET): $(DEBUG_OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

# Цель release
release: $(RELEASE_OBJS)
	$(CXX) $^ $(LDFLAGS) -o $(TARGET)

# Компиляция .cpp -> .o
build/debug/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) -MMD -c $< -o $@

build/release/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CFLAGS) -MMD -c $< -o $@

# Зависимости
-include build/debug/*.d build/release/*.d

# Очистка
clean:
	rm -rf build/ $(TARGET)
	find $(SRC_DIR) -type f \( -name '*.o' -o -name '*.d' \) -delete
