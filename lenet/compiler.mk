.PHONY : clean build all

MODEL ?= mnist
MODELINPUT ?= "Input3",float,[1,1,28,28]
PROFILE ?= mnist.yaml
PRECISION ?= Int16
BUNDLE ?= bundle
MEMOPT ?= false
CONVPAD ?= false
RECBIAS ?= true
DACTV ?= false

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
		-reuse-activation-memory-allocations=$(MEMOPT) \
		-recompute-conv-pads=$(CONVPAD) \
		-rescale-bias=$(RECBIAS) \
		-disable-fusing-actv=$(DACTV) \
		-backend=NMP \
		-dump-llvm-ir > $(MODEL).ll

clean:
	rm -f ${BUNDLE}/$(MODEL).o
