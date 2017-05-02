GPU=1
CUDNN=0
OPENCV=0
DEBUG=0

ARCH= -gencode arch=compute_30,code=sm_30 \
      -gencode arch=compute_35,code=sm_35 \
      -gencode arch=compute_50,code=[sm_50,compute_50] \
      -gencode arch=compute_52,code=[sm_52,compute_52] \
      -gencode arch=compute_53,code=[sm_53,compute_53]

VPATH=src/
OBJDIR=./obj/
LOCALOBJDIR=./obj/
INC=-Iinclude -Isrc/darknet

BINDIR=build/
EXEC=run_detection\
     run_batch\
     run_draw_boxes\
     run_gpu_test\

CUDA_DIR=/usr/local/cuda-8.0

CC=gcc
NVCC=$(CUDA_DIR)/bin/nvcc 
OPTS=-Ofast
LDFLAGS= -lm -pthread 
COMMON= 
CFLAGS=-Wall -Wfatal-errors -march=native -mtune=native

ifeq ($(DEBUG), 1) 
OPTS=-O0 -g
endif

CFLAGS+=$(OPTS)

ifeq ($(OPENCV), 1) 
COMMON+= -DOPENCV
CFLAGS+= -DOPENCV
LDFLAGS+= `pkg-config --libs opencv` 
COMMON+= `pkg-config --cflags opencv` 
endif

ifeq ($(GPU), 1) 
COMMON+= -DGPU -I$(CUDA_DIR)/include/
CFLAGS+= -DGPU
LDFLAGS+= -L$(CUDA_DIR)/lib64 -lcuda -lcudart -lcublas -lcurand
endif

ifeq ($(CUDNN), 1) 
COMMON+= -DCUDNN 
CFLAGS+= -DCUDNN
LDFLAGS+= -lcudnn
endif

OBJ=gemm.o utils.o cuda.o convolutional_layer.o list.o image.o activations.o im2col.o col2im.o blas.o crop_layer.o dropout_layer.o maxpool_layer.o softmax_layer.o data.o matrix.o network.o connected_layer.o cost_layer.o parser.o option_list.o detection_layer.o captcha.o route_layer.o writing.o box.o nightmare.o normalization_layer.o avgpool_layer.o coco.o dice.o yolo.o detector.o layer.o compare.o classifier.o local_layer.o swag.o shortcut_layer.o activation_layer.o rnn_layer.o gru_layer.o rnn.o rnn_vid.o crnn_layer.o demo.o tag.o cifar.o go.o batchnorm_layer.o art.o region_layer.o reorg_layer.o super.o voxel.o tree.o
LOCALOBJ=inference.o netutils.o vidutils.o frameio.o draw_box.o linked_list.o fileio.o
ifeq ($(GPU), 1) 
LDFLAGS+= -lstdc++ 
OBJ+=convolutional_kernels.o activation_kernels.o im2col_kernels.o col2im_kernels.o blas_kernels.o crop_layer_kernels.o dropout_layer_kernels.o maxpool_layer_kernels.o network_kernels.o avgpool_layer_kernels.o
endif

EXECS = $(addprefix $(BINDIR), $(EXEC))
OBJS = $(addprefix $(OBJDIR), $(OBJ))
LOCALOBJS = $(addprefix $(LOCALOBJDIR), $(LOCALOBJ))
DEPS = $(wildcard src/*.h) Makefile

all: build obj output $(EXECS)

$(BINDIR)run_draw_boxes: src/run_draw_boxes.c $(LOCALOBJS) $(OBJS)
	$(CC) $(COMMON) $(CFLAGS) $^ $(INC) -o $@ $(LDFLAGS)

$(BINDIR)run_gpu_test: src/run_gpu_test.c $(LOCALOBJS) $(OBJS)
	$(CC) $(COMMON) $(CFLAGS) $^ $(INC) -o $@ $(LDFLAGS)

$(BINDIR)run_batch: src/run_batch.c $(LOCALOBJS) $(OBJS)
	$(CC) $(COMMON) $(CFLAGS) $^ $(INC) -o $@ $(LDFLAGS)

$(BINDIR)run_detection: src/run_detection.c $(LOCALOBJS) $(OBJS)
	$(CC) $(COMMON) $(CFLAGS) $^ $(INC) -o $@ $(LDFLAGS)

$(LOCALOBJDIR)%.o: %.c $(DEPS)
	$(CC) $(COMMON) $(CFLAGS) $(INC) -c $< -o $@

$(OBJDIR)%.o: src/darknet/%.c $(DEPS)
	$(CC) $(COMMON) $(CFLAGS) $(INC) -c $< -o $@

$(OBJDIR)%.o: src/darknet/%.cu $(DEPS)
	$(NVCC) $(ARCH) $(COMMON) --compiler-bindir $(CC) --compiler-options "$(CFLAGS)" -c $< -o $@

build:
	mkdir -p build/

obj:
	mkdir -p obj

output:
	mkdir -p output

.PHONY: clean

clean:
	rm -rf build obj $(EXECS)
