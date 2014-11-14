CC=g++
DEBUG=
CFLAGS=-c -Wall $(DEBUG)
LDFLAGS=-lncursesw $(DEBUG)
SOURCES= Card.cpp Deck.cpp Player.cpp display.cpp ShowCribbage.cpp Cribbage.cpp main.cpp 	  #COMMENT ME IF YOU WANT TO RUN TESTS
#SOURCES= Card.cpp Deck.cpp Player.cpp display.cpp ShowCribbage.cpp testCribbage.cpp main.cpp #COMMENT ME IF YOU WANT TO RUN GAME
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=play

all: clean $(SOURCES) $(EXECUTABLE)

debug: clean
debug: DEBUG +=-g
debug: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *.o $(EXECUTABLE)

