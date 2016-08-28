CC=gcc
CFLAGS= -c -Wall -I /usr/include -Ofast -ffast-math -march=native -ggdb -masm=intel
LDFLAGS= -lboost_chrono -lboost_system -lm
SOURCES=src/main.c src/util.c src/vec2.c src/qtree/qtree_item.c src/qtree/qtree.c src/neural/neuron.c src/neural/neural_layer.c src/neural/neural_network.c  src/neural/util.c src/ant.c src/world.c
OBJECTS=$(SOURCES:.c=.o)
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
