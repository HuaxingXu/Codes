CC=g++
CCFLAGS=-g -Wall
LDFLAGS=-lasound -lm

all: play record
	
play: play.cpp
	$(CC) play.cpp $(CCFLAGS) $(LDFLAGS) -o play
	
record: record.cpp
	$(CC) record.cpp $(CCFLAGS) $(LDFLAGS) -o record