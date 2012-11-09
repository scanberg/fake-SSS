CC = g++
CFLAGS = -Wall -O3 -ffast-math
LIBS = -lglew32 -lglfw -lopengl32 -lglu32 -ldevil -lilu
SOURCES = main.cpp error.cpp shader.cpp txttools.cpp camera.cpp aseloader.cpp glrenderer.cpp gmoloader.cpp material.cpp matloader.cpp mesh.cpp model.cpp objloader.cpp
OBJDIR = objs
OBJECTS = $(SOURCES:.cpp=.o)
OBJ = $(patsubst %,$(OBJDIR)/%,$(OBJECTS))
EXECUTABLE = run

all: $(SOURCES) $(EXECUTABLE)
	./$(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJ) -o "$(EXECUTABLE)" $(LIBS)

.cpp.o:
	$(CC) $(CFLAGS) -c $< -o $(OBJDIR)/$@