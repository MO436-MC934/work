.PHONY:: all build clean

MODEL ?= mobilenet
MAIN  ?= main

# Toolchain
CXX =clang++
CXXFLAGS = -O2 -Wall
LIBS := -lpng -lopenblas

BUNDLE_DIR := ./bin
BUILD_DIR := ./bin
SRC_DIR := ./

build: $(BUILD_DIR)/$(MAIN).x

$(BUILD_DIR)/$(MAIN).x: $(SRC_DIR)/$(MAIN).o $(BUNDLE_DIR)/$(MODEL).o
	$(CXX) -o $@ $^ $(LIBS) $(CXXFLAGS)

$(SRC_DIR)/$(MAIN).o : $(SRC_DIR)/$(MAIN).cpp $(BUNDLE_DIR)/$(MODEL).h
	$(CXX) $< -c -o $@ -I$(BUNDLE_DIR) $(CXXFLAGS)

clean:
	rm -rf $(BUILD_DIR)/$(MAIN).x
	rm -rf $(SRC_DIR)/$(MAIN).o


