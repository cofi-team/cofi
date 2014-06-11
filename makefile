CC=g++
CFLAGS=-c -Wall
LIB_DIR=/usr/local/lib
LDFLAGS=-lpqxx -lpq -L$(LIB_DIR)
SOURCES=main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=cofi

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o: 
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o cofi
