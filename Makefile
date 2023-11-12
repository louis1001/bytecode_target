SRC_FILES = src/main.c src/program_builder.c src/program.c src/vm.c src/opcodes.c
BUILD_DIR = build
CC_FLAGS = -Wall -Wpedantic -Wextra -Wno-variadic-macros -Werror -g -std=c11
INCLUDES = -Iinclude
BUILD_OPTIONS = -DDEBUG=1 -DVERBOSE=0
CC = clang

all: build prog

build: 
	mkdir -p ${BUILD_DIR}

prog:
	${CC} ${SRC_FILES} ${CC_FLAGS} ${BUILD_OPTIONS} -o ${BUILD_DIR}/vm ${INCLUDES}

sanitize:
	${CC} ${SRC_FILES} ${CC_FLAGS} ${BUILD_OPTIONS} -o ${BUILD_DIR}/vm ${INCLUDES} -fsanitize=address -fno-omit-frame-pointer -g -O0

clean:
	rm -rf ${BUILD_DIR}/**