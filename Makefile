ENGINE_NAME := Overture
ENGINE_VERSION_MAJOR := 0
ENGINE_VERSION_MINOR := 0
ENGINE_VERSION_PATCH := 0

# doesn't make sense if i have an SO_DIRS list which is a list of projects 
PROJECT_NAME := Overture Test
VERSION_MAJOR := 0
VERSION_MINOR := 0
VERSION_PATCH := 0

SRC_DIRS := src src/core src/platform src/graphics
INCLUDE_DIRS := include src
SO_DIRS := 
EXAMPLES_DIR := examples
BUILD_DIR := build
BIN_DIR := bin

OBJS := $(patsubst %.c,%.o, $(foreach SRC_DIR,$(SRC_DIRS),$(wildcard $(SRC_DIR)/*.c)))
EXAMPLES := $(patsubst %/,%, $(sort $(dir $(wildcard $(EXAMPLES_DIR)/*/)))) #dont know if this is a good idea

CC := gcc

CFLAGS := -g -fPIC -DVERSION_MAJOR=$(VERSION_MAJOR) -DVERSION_MINOR=$(VERSION_MINOR) -DVERSION_PATCH=$(VERSION_PATCH) -DENGINE_VERSION_MAJOR=$(ENGINE_VERSION_MAJOR) -DENGINE_VERSION_MINOR=$(ENGINE_VERSION_MINOR) -DENGINE_VERSION_PATCH=$(ENGINE_VERSION_PATCH) -DENGINE_NAME='"$(ENGINE_NAME)"' -DPROJECT_NAME='"$(PROJECT_NAME)"'
LDFLAGS := -lglfw -lvulkan

default: examples/02_triangle

$(SO_DIRS): dir $(OBJS)
	$(CC) $(CFLAGS) $(foreach INCLUDE_DIR,$(INCLUDE_DIRS),-I$(INCLUDE_DIR)) -o $(BIN_DIR)/$(notdir $@).o -c $(wildcard $@/*.c)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN_DIR)/$(notdir $@) $(patsubst %, build/%, $(OBJS)) $(BIN_DIR)/$(notdir $@).so
# -rdynamic to be able to load all symbols in program i think, honestly idk but my small test points towards it
#  this might affect performance tho or smt stupid idk, maybe not tho

$(EXAMPLES): dir $(OBJS)
	$(CC) $(CFLAGS) $(foreach INCLUDE_DIR,$(INCLUDE_DIRS),-I$(INCLUDE_DIR)) -shared -o $(BIN_DIR)/$(notdir $@).so $(wildcard $@/*.c)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN_DIR)/$(notdir $@) $(patsubst %, build/%, $(OBJS)) $(BIN_DIR)/$(notdir $@).so

$(OBJS): dir
	@mkdir -p $(BUILD_DIR)/$(@D)
	@$(CC) $(CFLAGS) $(foreach INCLUDE_DIR,$(INCLUDE_DIRS),-I$(INCLUDE_DIR)) -o $(BUILD_DIR)/$@ -c $*.c

dir:
	@mkdir -p $(BUILD_DIR) $(BIN_DIR)

clean:
	@rm -rf $(BUILD_DIR) $(BIN_DIR)

bear:
	bear -- make $(EXECUTABLE)

.PHONY: dir clean bear
