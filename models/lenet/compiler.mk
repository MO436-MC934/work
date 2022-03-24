.PHONY : clean build all

MODEL ?= lenet
MODELINPUT ?= "conv2d_input",float,[1,1,28,28]
BUNDLE ?= bin
MEMOPT ?= false

all: clean build

build: ${BUNDLE}/$(MODEL).o

${BUNDLE}/$(MODEL).o : $(MODEL).onnx
	@echo 'Build the bundle object $@:'
	${GLOWBIN}/model-compiler \
		-model=$(MODEL).onnx \
		-model-input=$(MODELINPUT) \
		-onnx-define-symbol=unk__26,1 \
		-emit-bundle=$(BUNDLE) \
		-dump-graph-DAG-before-compile=$(MODEL)-before.dot \
		-dump-graph-DAG=$(MODEL)-after.dot \
		-reuse-activation-memory-allocations=$(MEMOPT) \
        -backend=CPU \
		-dump-ir > $(MODEL).lir

clean:
	rm -f ${BUNDLE}/$(MODEL).o
