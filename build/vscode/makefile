#source file search paths
VPATH = ../../src 

# folder for .o files
ODIR=./obj

# sources
_CIN = cintest.o 
CIN = $(patsubst %,$(ODIR)/%,$(_CIN))

_SERVER = tcpserver.o
SERVER = $(patsubst %,$(ODIR)/%,$(_SERVER))

_CLIENT = tcpclient.o
CLIENT = $(patsubst %,$(ODIR)/%,$(_CLIENT))

$(ODIR)/%.o: %.cpp 
	g++ -g  -std=c++17 -I../../include \
	-c -o $@ $<

cin: $(CIN)
	g++ -g \
	-o ../../bin/cintest.exe $^

server: $(SERVER)
	g++ -g  \
	-o ../../bin/tcpserver.exe $^ -lws2_32 -lwsock32

client: $(CLIENT)
	g++ -g  \
	-o ../../bin/tcpclient.exe $^ -lws2_32 -lwsock32

.PHONY: clean
clean:
	del /q .\obj


