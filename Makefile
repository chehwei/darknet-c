TARGETS=lib/libdarknet-c.so lib/libdarknet-c.a  
TOOLS=bin/convert_cfg_to_json
TEST_TARGETS=

DEBUG 		?= 1
OPTIMIZE 	?= 0

USE_OPENMP 	?= 0
USE_CUDA	?= 0
USE_OPENCL	?= 0

USE_OPENCV	?= 0


CC=gcc -std=gnu99
AR=ar
LINKER=gcc -std=gnu99

CFLAGS=-Wall
LDFLAGS=
LIBS=-lm -lpthread


ifeq ($(DEBUG),1)
	CFLAGS += -g -D_DEBUG
	OPT=-O0
else
	OPT=-Ofast
endif

ifeq ($(USE_OPENMP),1)
	CFLAGS += -D_USE_OPENMP
	LDFLAGS += -fopenmp
endif


SOURCES	:= $(wildcard src/*.c)
OBJECTS := $(SOURCES:src/%.c=obj/%.o)
STATIC_OBJECTS := $(SOURCES:src/%.c=obj/static/%.o)


TEST_SOURCES := $(wildcard test/*.c)
TEST_OBJECTS := $(TEST_SOURCES:test/%.c=obj/test/%.o)

TOOLS_SOURCES := $(wildcard tools/*.c)
TOOLS_OBJECTS := $(TOOLS_SOURCES:tools/%.c=obj/tools/%.o)


all: do_init $(TARGETS) $(TOOLS) $(TEST_TARGETS)

lib/libdarknet-c.so: $(OBJECTS)
	$(LINKER) $(LDFLAGS) $(OPT) -fPIC -shared -o $@ $(OBJECTS) $(LIBS)
	
lib/libdarknet-c.a: $(STATIC_OBJECTS)
	$(AR) crf $@ $(STATIC_OBJECTS)
	
bin/convert_cfg_to_json: $(TOOLS_OBJECTS)
	$(LINKER) $(LDFLAGS) -o $@ $(TOOLS_OBJECTS) -ljson-c -lm -lpthread


$(OBJECTS): obj/%.o : src/%.c
	$(CC) -fPIC -o $@ -c $< $(CFLAGS)
	
$(STATIC_OBJECTS): obj/static/%.o : src/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

$(TEST_OBJECTS): obj/test/%.o : test/%.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
$(TOOLS_OBJECTS): obj/tools/%.o : tools/%.c
	$(CC) -o $@ -c $< $(CFLAGS)
	
.PHONY: do_init clean
do_init:
	mkdir -p bin obj obj/test obj/tools lib
	
clean:
	rm -f obj/*.o obj/test/*.o obj/static/*.o obj/tools/*.o $(TARGETS) $(TOOLS) $(TEST_TARGETS)
	

	
