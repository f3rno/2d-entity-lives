CC=g++
CFLAGS=-std=c++11 -c -Wall -I /usr/include -Ofast -ffast-math -march=native -ggdb
LDFLAGS= -lboost_chrono -lboost_system
SOURCES=src/main.cpp src/qtree/qtree_item.cpp src/qtree/qtree.cpp src/neural/neuron.cpp src/neural/neural_layer.cpp src/neural/neural_network.cpp src/vector2.cpp src/neural/utility.cpp src/ant.cpp src/world.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=bin/networks

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm src/*.o
	rm src/neural/*.o
	rm src/qtree/*.o
