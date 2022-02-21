.PHONY : clean build all

MODEL ?= mnist
MODELINPUT ?= "Input3"
PROFILE ?= $(MODEL).yaml
IMAGEMODE ?= 0to1

all: clean build

build: $(PROFILE)

$(PROFILE):
	@echo 'Build the $(MODEL) profiler $@:'
	${GLOWBIN}/image-classifier images/*.png \
		-image-layout=NCHW -image-mode=$(IMAGEMODE) \
		-model=$(MODEL).onnx -model-input-name=$(MODELINPUT) \
		-dump-profile=$@

clean:
	rm -f $(PROFILE)
