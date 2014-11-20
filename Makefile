CC=g++
CFLAGS=-std=c++11 -c -Wall -I /usr/include
LDFLAGS= -lsfml-window -lsfml-graphics -lsfml-system -lboost_thread -lboost_chrono -lboost_system
SOURCES=src/main.cpp src/neuron.cpp src/neural_layer.cpp src/neural_network.cpp src/vector2.cpp src/utility.cpp src/entity.cpp src/ant.cpp src/food.cpp src/world.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=networks

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
