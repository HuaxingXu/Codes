CC=g++
CCFLAGS=-Wall
LDFLAGS= -pthread -lspeex -lm -lasound

all: udpchat 
	
udpchat: udpchat.cpp Voice.cpp Voice.h UdpTransfer.cpp UdpTransfer.h UdpChat.cpp UdpChat.h Recorder.cpp Recorder.h Player.cpp Player.h
	$(CC) udpchat.cpp Voice.cpp UdpTransfer.cpp UdpChat.cpp Recorder.cpp Player.cpp $(CPPFLAGS) $(CCFLAGS) $(LDFLAGS) -o udpchat
	
clean:
	rm udpchat
