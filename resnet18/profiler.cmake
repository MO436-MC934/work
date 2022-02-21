.PHONY : clean build all

MODEL ?= resnet18
MODELINPUT ?= "data"
PROFILE ?= $(MODEL).yaml
IMAGEMODE ?= 0to1
ORDER ?= RGB
CALIBRATION ?= ../calibration/images224

all: clean build

build: $(PROFILE)

$(PROFILE):
	@echo 'Build the $(MODEL) profiler $@:'
	${GLOWBIN}/image-classifier ${CALIBRATION}/*.png \
		-image-layout=NCHW \
		-image-mode=$(IMAGEMODE) \
		-image-channel-order=$(ORDER) \
		-use-imagenet-normalization \
		-model=$(MODEL).onnx \
		-model-input-name=$(MODELINPUT) \
		-dump-profile=$@

clean:
	rm -f $(PROFILE)
