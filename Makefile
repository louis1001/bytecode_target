SRC_FILES = src/main.c src/program_builder.c src/program.c src/vm.c src/opcodes.c src/core.c src/assembler.c
BUILD_DIR = build
SYM_PATH = ./vm
CC_FLAGS = -Wall -Wpedantic -Wextra -Wno-variadic-macros -Wimplicit-fallthrough -Werror -g -std=c11
INCLUDES = -Iinclude
BUILD_OPTIONS = -DDEBUG=0 -DVERBOSE=0
CC = clang

all: build prog link

run: build exec

exec:
	${BUILD_DIR}/vm

build: 
	mkdir -p ${BUILD_DIR}

prog:
	${CC} ${SRC_FILES} ${CC_FLAGS} ${BUILD_OPTIONS} -o ${BUILD_DIR}/vm ${INCLUDES}

link:
	rm -f ${SYM_PATH} && ln -s ${BUILD_DIR}/vm ${SYM_PATH}

sanitize:
	${CC} ${SRC_FILES} ${CC_FLAGS} ${BUILD_OPTIONS} -o ${BUILD_DIR}/vm ${INCLUDES} -fsanitize=address -fno-omit-frame-pointer -g -O0

clean:
	rm -rf ${BUILD_DIR}/** && rm -f ${SYM_PATH}
