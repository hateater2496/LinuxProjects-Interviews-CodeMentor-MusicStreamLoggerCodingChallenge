INSTALL_DIR ?= install
BIN_DIR = $(INSTALL_DIR)/bin
TEST_DIR = $(INSTALL_DIR)/tests

OUT = $(BIN_DIR)/MusicStreamLogger
TEST = $(TEST_DIR)/TestMusicStream

LINK=-Wl

INCLUDES  = src/Logger.h
INCLUDES += src/MusicStreamMonitor.h
INCLUDES += src/MusicStreamLogger.h

CPP_FILES  = src/Logger.cpp
CPP_FILES += src/MusicStreamMonitor.cpp
CPP_FILES += src/MusicStreamLogger.cpp
CPP_FILES += src/main.cpp

TEST_FILES  = src/Logger.cpp
TEST_FILES += src/MusicStreamMonitor.cpp
TEST_FILES += src/MusicStreamLogger.cpp
TEST_FILES += src/TestMusicStream.cpp

INCDIRS = -Iext

# C++ compiler flags (-g -O2 - Wall) & ignoring unknown-pragmas for cmdparser
CCFLAGS = $(INCDIRS) -g -O2 -Wall -Werror -std=c++14 -Wno-unknown-pragmas
# compiler
CCC = g++

$(OUT): $(CPP_FILES) $(INCLUDES)
	$(CCC) $(LINK),-rpath,\$$ORIGIN $(CCFLAGS) $(CPP_FILES) -o $(OUT)

$(TEST): $(TEST_FILES) $(INCLUDES)
	$(CCC) $(LINK),-rpath,\$$ORIGIN $(CCFLAGS) $(TEST_FILES) -o $(TEST)

clean:
	rm -f $(OUT) $(TEST)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TEST_DIR):
	mkdir -p $(TEST_DIR)

install: $(BIN_DIR) $(TEST_DIR) $(OUT) $(TEST)

