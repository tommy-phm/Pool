# Compiler
CC := g++
WINDRES := windres

# Compiler flags
CFLAGS := -g
INCLUDE_DIRS := -I./include
LIB_DIRS := -L./lib

# Icon Resource
RESOURCE_OBJ = resource/resource.o

# Source files
SRC_FILES := src/main.cpp src/logic.cpp src/network.cpp src/graphics.cpp src/glad.c

# Output executable name
OUTPUT := pool_simulator

# Windows-specific settings
ifeq ($(OS),Windows_NT)
	OBJECTS += $(RESOURCE_OBJ)
    OUTPUT := $(OUTPUT).exe
    LIBS := -lglfw3dll -lws2_32
else
# Linux-specific settings
    LIBS := -lglfw
endif

all: $(OUTPUT)

$(OUTPUT): $(SRC_FILES)
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) $(LIB_DIRS) $(RESOURCE_OBJ) $(SRC_FILES) $(LIBS) -o $(OUTPUT)

.PHONY: clean

clean:
	rm -f $(OUTPUT)

