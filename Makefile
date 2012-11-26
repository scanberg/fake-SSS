OS = $(shell uname -s)
CC = g++
MV = mv
CP = cp
RM = rm
MESSAGE =
OPENGL=1

#specify dependency list
OBJECTS ?= src/main.o src/Engine.o src/Geometry.o src/ObjLoader.o src/Shader.o src/Tokenizer.o src/Camera.o src/Log.o
OUTPUT ?= Program
FLAGS ?=
CFLAGS ?= -O3 -fopenmp
INCPATH ?=

# check if argument OPENGL=1 is set, reguires glfw to be properly installed
ifdef OPENGL
	MESSAGE += OpenGL,
	ifeq ($(OS),Linux)
		# CPPFLAGS := -lGL -lGLU -lX11 -static-libgcc -static-libstdc++
		FLAGS += -lglfw
	else ifeq ($(OS),Darwin)
		FLAGS += -framework Cocoa -framework OpenGL -lGLEW -lglfw
	endif
endif



all: $(OBJECTS)
	-@echo "Linking for $(OS)"
	-@echo "Including $(MESSAGE)"
	-@echo " "
	$(CC) -o $(OUTPUT) $(OBJECTS) $(FLAGS) $(CFLAGS) $(INCPATH)

clean:
	-@echo "Cleaning"
	-$(RM) src/*.o

# pattern that compiles all .o files, both .cpp and .c works (tested on linux)
%.o: %.cpp
	$(CC) -c $(CFLAGS) $(CPPLAGS) $< -o $@


