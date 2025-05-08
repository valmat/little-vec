#CPP             = g++
#LD              = g++
#CPP             = clang++
#LD              = clang++

CPP             = g++
#CPP             = clang++

LD = $(CPP)

OPT_FLAG=-O3 -pedantic -Wall -Wextra -fno-rtti -fno-exceptions -flto=auto
# OPT_FLAG=-O3 -pedantic -Wall -Wextra -Wno-unused-parameter -fno-rtti -fno-exceptions
#OPT_FLAG=-g -O3

NAME       = little_vec

CPP_STD    = -std=c++17
CPP_FLAGS  = -I. -I"/usr/include/rocksserver/rocksdb" ${OPT_FLAG} ${CPP_STD} -fPIC -c
LD_FLAGS   = ${CPP_STD} ${OPT_FLAG} -shared


RESULT     = $(NAME).so

SOURCES = $(shell find . -type f -name '*.cpp')
OBJECTS = $(patsubst ./%.cpp,o/%.o,$(SOURCES))


all: ${RESULT}

${RESULT}: ${OBJECTS}
	${LD} ${LD_FLAGS} -o $@ ${OBJECTS}
	strip ${RESULT}


${OBJECTS} : o/%.o : ./%.cpp
	@mkdir -p $(dir $(@))
	${CPP} ${CPP_FLAGS} $< -o $@


clean:
	rm -f *.so
	find o/ -type f -name '*.o' -delete


