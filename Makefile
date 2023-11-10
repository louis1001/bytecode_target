SRC_FILES = src/main.c src/program_builder.c src/program.c src/vm.c
BUILD_DIR = build
CC_FLAGS = -Wall -Wextra -g -std=c11
INCLUDES = -Iinclude
BUILD_OPTIONS = -DDEBUG=1 -DVERBOSE=1
CC = clang

all: build prog

build: 
	mkdir -p ${BUILD_DIR}

prog:
	${CC} ${SRC_FILES} ${CC_FLAGS} ${BUILD_OPTIONS} -o ${BUILD_DIR}/vm ${INCLUDES}

clean:
	rm -rf ${BUILD_DIR}/**