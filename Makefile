CC=g++
CFLAGS=-std=c++11 -c -Wall -I /usr/include -Ofast -ffast-math -march=native
LDFLAGS= -lsfml-window -lsfml-graphics -lsfml-system -lboost_chrono -lboost_system
SOURCES=src/main.cpp src/neural/neuron.cpp src/neural/neural_layer.cpp src/neural/neural_network.cpp src/vector2.cpp src/neural/utility.cpp src/entity.cpp src/ant.cpp src/food.cpp src/world.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/networks

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
