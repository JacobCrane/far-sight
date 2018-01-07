CC=g++
CFLAGS=-c -Wall -g -std=gnu++14 -Iinc
LDFLAGS=
SOURCES=src/naughts_and_crosses.cpp main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=main.out
.PHONY=clean

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(EXECUTABLE) $(OBJECTS)

