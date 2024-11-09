# ==================================================================================================
# 1. Compiler flags (e.g., -Wall, -Wextra, -pedantic, etc.)
# 2. Include directories (-I./libs/)
# 3. Source files (./src/*.c)
# 4. Binary output
# 5. Linker flags (e.g., -static, -static-libgcc, -static-libstdc++)
# 6. Libraries (-lX11)

CC = g++
DBG_BIN = gdb
PLATFORM ?= sdl

# CFLAGS = -D_GNU_SOURCE
# CFLAGS += -std=gnu11
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
# CFLAGS += -Werror
CFLAGS += -Wmissing-declarations
ASANFLAGS=-fsanitize=address -fno-common -fno-omit-frame-pointer
INCS = -I./libs/
BIN_DIR = ./bin
BIN = $(BIN_DIR)/game
LDFLAGS =
LIBS =

ifeq ($(PLATFORM),sdl)
	INCS += $(shell pkg-config --cflags sdl2 SDL2_ttf SDL2_image)
	SRC = $(filter-out ./src/lin_main.cpp, $(wildcard ./src/*.cpp))
	LIBS += $(shell pkg-config --libs sdl2 SDL2_ttf SDL2_image)
endif

ifeq ($(PLATFORM),lin)
	INCS += $(shell pkg-config --cflags x11)
	SRC = $(filter-out ./src/sdl_main.cpp, $(wildcard ./src/*.cpp))
	LIBS += $(shell pkg-config --libs x11)
endif

ifeq ($(shell uname -s),Darwin)
	DBG_BIN = lldb
endif

build: bin-dir
	$(CC) $(CFLAGS) $(INCS) $(SRC) -o $(BIN)-$(PLATFORM) $(LDFLAGS) $(LIBS) 

bin-dir:
	mkdir -p $(BIN_DIR)

debug: debug-build
	$(DBG_BIN) $(BIN) $(ARGS)

debug-build: bin-dir
	$(CC) $(CFLAGS) -g $(INCS) $(SRC) -o $(BIN)-$(PLATFORM) $(LDFLAGS) $(LIBS)

run: build
	@$(BIN)-$(PLATFORM) $(ARGS)

memcheck-lin:
	@$(CC) -g $(SRC) $(ASANFLAGS) $(CFLAGS) $(INCS) $(SRC) $(LDFLAGS) $(LIBS) -o memcheck.out
	@./memcheck.out
	@echo "Memory check passed"

clean:
	rm -rf $(BIN_DIR)/* 

gen-compilation-db:
	PLATFORM=sdl bear -- make build
	mv compile_commands.json compile_commands_sdl.json
	PLATFORM=lin bear -- make build
	mv compile_commands.json compile_commands_lin.json

