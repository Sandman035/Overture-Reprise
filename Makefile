NAME := overture_reprise
SRC_DIRS := src src/core test
INCLUDE_DIRS := include
SO_DIRS := test
BUILD_DIR := build
BIN_DIR := bin

OBJS := $(patsubst %.c,%.o, $(foreach SRC_DIR,$(SRC_DIRS),$(wildcard $(SRC_DIR)/*.c)))

CC := gcc

CFLAGS := -g -fPIC
LDFLAGS :=

$(NAME): dir $(OBJS)
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
	bear -- make $(NAME)

.PHONY: dir clean bear
