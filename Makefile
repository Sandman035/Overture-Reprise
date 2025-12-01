SRC_DIRS := src src/core test
ENGINE_NAME := Overture
ENGINE_VERSION_MAJOR := 0
ENGINE_VERSION_MINOR := 0
ENGINE_VERSION_PATCH := 0

PROJECT_NAME := Overture Test
VERSION_MAJOR := 0
VERSION_MINOR := 0
VERSION_PATCH := 0

EXECUTABLE := overture
INCLUDE_DIRS := include
SO_DIRS := test
BUILD_DIR := build
BIN_DIR := bin

OBJS := $(patsubst %.c,%.o, $(foreach SRC_DIR,$(SRC_DIRS),$(wildcard $(SRC_DIR)/*.c)))

CC := gcc

LDFLAGS :=
CFLAGS := -g -fPIC -DVERSION_MAJOR=$(VERSION_MAJOR) -DVERSION_MINOR=$(VERSION_MINOR) -DVERSION_PATCH=$(VERSION_PATCH) -DENGINE_VERSION_MAJOR=$(ENGINE_VERSION_MAJOR) -DENGINE_VERSION_MINOR=$(ENGINE_VERSION_MINOR) -DENGINE_VERSION_PATCH=$(ENGINE_VERSION_PATCH) -DENGINE_NAME='"$(ENGINE_NAME)"' -DPROJECT_NAME='"$(PROJECT_NAME)"'

$(EXECUTABLE): dir $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN_DIR)/$@ $(patsubst %, build/%, $(OBJS))

$(OBJS): dir
	@mkdir -p $(BUILD_DIR)/$(@D)
	@$(CC) $(CFLAGS) $(foreach INCLUDE_DIR,$(INCLUDE_DIRS),-I$(INCLUDE_DIR)) -o $(BUILD_DIR)/$@ -c $*.c

$(SO_DIRS): dir
	$(CC) $(CFLAGS) $(foreach INCLUDE_DIR,$(INCLUDE_DIRS),-I$(INCLUDE_DIR)) -shared -o $(BIN_DIR)/$@.so $(wildcard $@/*.c)

dir:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

bear:
	bear -- make $(EXECUTABLE)

.PHONY: dir clean bear
