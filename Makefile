CC = g++

BUILD_DIR = build
OUT = $(BUILD_DIR)/opencl_example

KERNEL_FILE_PATH = $(SRC_DIR)/kernel.cl

CFLAGS = -std=c++17 -Wall -Wpedantic -O2 -DKERNEL_FILE_PATH=$(KERNEL_FILE_PATH)
LFLAGS = -O2 -lOpenCL

SRC_DIR = src
INC_DIR = src


SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SOURCES))

all: $(OUT)

clean:
	rm -rf $(BUILD_DIR)/*

$(OUT): $(OBJECTS)
	$(CC) -o $@ $? $(LFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp build
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

build:
	mkdir build
