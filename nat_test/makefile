CC=g++
CCFLAGS=-Wall
LDFLAGS=-pthread 

all: client 

client:
	$(CC) client.cpp UdpSocket.cpp $(CCFLAGS) $(LDFLAGS) -o client

clean:
	rm client 
