.PHONY : clean build all

MODEL ?= resnet18
MODELINPUT ?= "data",float,[1,3,224,224]
PROFILE ?= $(MODEL).yaml
PRECISION ?= Int16
BUNDLE ?= bundle
CALIBRATE ?= false
MEMOPT ?= false
CONVPAD ?= false
POOLPAD ?= false
RECBIAS ?= false
DFACTV ?= false
HALT ?= 0

all: clean build

build: ${BUNDLE}/$(MODEL).o

${BUNDLE}/$(MODEL).o : $(PROFILE)
	@echo 'Build the bundle object $@:'
	${GLOWBIN}/model-compiler \
		-load-profile=$< \
		-model=$(MODEL).onnx \
		-model-input=$(MODELINPUT) \
		-emit-bundle=$(BUNDLE) \
		-quantization-schema=symmetric_with_power2_scale \
		-quantization-precision=$(PRECISION) \
		-quantization-precision-bias=$(PRECISION) \
		-dump-graph-DAG=$(MODEL)-quant.dot \
		-backend=NMP \
        -calibrate-constants=$(CALIBRATE) \
		-disable-fusing-actv=$(DFACTV) \
		-reuse-activation-memory-allocations=$(MEMOPT) \
		-recompute-conv-pads=$(CONVPAD) \
		-recompute-pool-pads=$(POOLPAD) \
		-rescale-bias=$(RECBIAS) \
		-halt-layer=$(HALT) \
		-dump-llvm-ir > $(MODEL).ll

clean:
	rm -f ${BUNDLE}/$(MODEL).o
