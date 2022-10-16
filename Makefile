obj_files = \
	src/main.o

build_dir = build
target_dir = $(build_dir)/ultvyu

CC = gcc
CFLAGS =
LIBS = -lSDL2 -lm
TARGET = $(target_dir)/ultvyu
run_method = ./ultvyu

all: build run

build: $(obj_files)
	mkdir -p $(target_dir)
	$(CC) $(obj_files) $(CFLAGS) $(LIBS) -o $(TARGET)

run:
	cd $(target_dir); $(run_method); cd ../../
	
clean:
	rm $(obj_files)
