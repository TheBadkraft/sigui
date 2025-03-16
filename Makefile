CC = gcc
CFLAGS = -Wall -g -fPIC -I$(INCLUDE_DIR)
LDFLAGS = -shared
TST_CFLAGS = $(CFLAGS) -DTSTDBG
TST_LDFLAGS = -lsigcore -lsigtest -L/usr/lib

SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
BIN_DIR = bin
LIB_DIR = $(BIN_DIR)/lib
TEST_DIR = test
TST_BUILD_DIR = $(BUILD_DIR)/test

CORE_SRCS = $(filter-out $(SRC_DIR)/main.c, $(wildcard $(SRC_DIR)/*.c))
CORE_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(CORE_SRCS))
TST_SRCS = $(wildcard $(TEST_DIR)/*.c)
TST_OBJS = $(patsubst $(TEST_DIR)/%.c, $(TST_BUILD_DIR)/%.o, $(TST_SRCS))
MAIN_OBJ = $(BUILD_DIR)/main.o

HEADER = $(INCLUDE_DIR)/sigui.h
SRC_HEADERS = $(wildcard $(SRC_DIR)/*.h)

LIB_TARGET = $(LIB_DIR)/libsigui.so
TST_TARGET = $(TST_BUILD_DIR)/run_tests
MAIN_TARGET = $(BIN_DIR)/main

all: $(CORE_OBJS)
lib: $(LIB_TARGET)
main: $(MAIN_TARGET)

$(LIB_TARGET): $(CORE_OBJS)
	@mkdir -p $(LIB_DIR)
	$(CC) $(CORE_OBJS) -o $(LIB_TARGET) $(LDFLAGS)

$(MAIN_TARGET): $(MAIN_OBJ) $(CORE_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(MAIN_OBJ) $(CORE_OBJS) -o $(MAIN_TARGET) $(TST_LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(HEADER) $(SRC_HEADERS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TST_BUILD_DIR)/%.o: $(TEST_DIR)/%.c $(HEADER) $(SRC_HEADERS)
	@mkdir -p $(TST_BUILD_DIR)
	$(CC) $(TST_CFLAGS) -c $< -o $@

$(TST_TARGET): $(TST_OBJS) $(CORE_OBJS)
	@mkdir -p $(TST_BUILD_DIR)
	$(CC) $(TST_OBJS) $(CORE_OBJS) -o $(TST_TARGET) $(TST_LDFLAGS)

$(TST_BUILD_DIR)/test_%: $(TST_BUILD_DIR)/test_%.o $(CORE_OBJS)
	@mkdir -p $(TST_BUILD_DIR)
	$(CC) $< $(CORE_OBJS) -o $@ $(TST_LDFLAGS)

install: $(LIB_TARGET) $(HEADER)
	sudo cp $(LIB_TARGET) $(INSTALL_LIB_DIR)/
	sudo cp $(INCLUDE_DIR)/sigui.h $(INSTALL_INCLUDE_DIR)/
	sudo ldconfig

test: $(TST_TARGET)
	@$(TST_TARGET)

test_%: $(TST_BUILD_DIR)/test_%
	@echo "Running test: $<"
	@$<

clean:
	find $(BUILD_DIR) -type f -delete
	find $(BIN_DIR) -type f -delete

.PHONY: all lib main clean install test test_%
