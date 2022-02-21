.PHONY:: all build clean

MODEL ?= resnet18

# Toolchain
CXX =aarch64-linux-gnu-g++
CXXFLAGS = --static -lstdc++

# NMP configuration
DFINE = #-DPOLLING
IDIR = -I${OPENCV_PATH}/include
LDIR = -L${OPENCV_PATH}/lib
LIBS := -lopencv_contrib -lopencv_stitching -lopencv_nonfree -lopencv_superres -lopencv_ocl -lopencv_ts -lopencv_videostab -lopencv_gpu -lopencv_photo -lopencv_objdetect -lopencv_legacy -lopencv_video -lopencv_ml -lopencv_calib3d -lopencv_features2d -lopencv_highgui -lIlmImf -llibjasper -llibtiff -llibpng -llibjpeg -lopencv_imgproc -lopencv_flann -lopencv_core -lzlib -lrt -lpthread -lm -ldl -lstdc++

BUILD_DIR := ./bin
BUNDLE_DIR := ./bundle
SRC_DIR := ./

build: $(BUILD_DIR)/$(MODEL).exe
	
$(BUILD_DIR)/$(MODEL).exe: $(SRC_DIR)/$(MODEL).o
	$(CXX) -o $@ $^ $(IDIR) $(LDIR) $(LIBS) $(CXXFLAGS) $(DFINE)
	cp $(BUNDLE_DIR)/$(MODEL).weights.bin $(BUILD_DIR)
	cp $(SRC_DIR)/$(MODEL).text.bin $(BUILD_DIR)

$(SRC_DIR)/$(MODEL).o : $(SRC_DIR)/$(MODEL).cpp $(BUNDLE_DIR)/$(MODEL).h
	$(CXX) $(CXXFLAGS) $(IDIR) $< -c -o $@

clean:
	rm -rf $(BUILD_DIR)/$(MODEL).exe
	rm -rf $(BUILD_DIR)/$(MODEL).weights.bin
	rm -rf $(BUILD_DIR)/$(MODEL).text.bin
	rm -rf $(SRC_DIR)/$(MODEL).o


