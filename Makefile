PYTHON = .venv/bin/python3

# Параметры Python
PY_CFLAGS  := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_config_var('CFLAGS') or '')")
PY_INCLUDE := $(shell $(PYTHON) -c "import sysconfig; print('-I' + sysconfig.get_path('include'))")

# Компилятор
CC = gcc

# Общие флаги
INCLUDE_DIRS := $(shell find $(SRC_DIR) -type d -not -path '*/.venv/*' -not -path '*/.git/*' -not -path '*/__pycache__/*' -exec echo -I{} \;)
BASE_CFLAGS  = -fPIC -Wall -Wextra $(PY_CFLAGS) $(PY_INCLUDE) $(INCLUDE_DIRS)
BASE_LDFLAGS = -shared

# Режимы
DEBUG_CFLAGS   = -g -O0
RELEASE_CFLAGS = -O3 -flto -fomit-frame-pointer -DNDEBUG
RELEASE_LDFLAGS = -flto

# Исходники
SRC_DIR = wfz_pocketworld
SOURCES = $(shell find $(SRC_DIR) -name '*.c')
TARGET  = $(SRC_DIR)/_core.so

# Объектные файлы для разных режимов
DEBUG_OBJS   = $(patsubst $(SRC_DIR)/%.c, build/debug/%.o, $(SOURCES))
RELEASE_OBJS = $(patsubst $(SRC_DIR)/%.c, build/release/%.o, $(SOURCES))

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
	$(CC) $^ $(LDFLAGS) -o $@

# Цель release
release: $(RELEASE_OBJS)
	$(CC) $^ $(LDFLAGS) -o $(TARGET)

# Компиляция .c -> .o
build/debug/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

build/release/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Зависимости
-include build/debug/*.d build/release/*.d

# Очистка
clean:
	rm -rf build/ $(TARGET)
	find $(SRC_DIR) -type f \( -name '*.o' -o -name '*.d' \) -delete
