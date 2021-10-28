# source file search paths
VPATH = ../../src  ../../../raven-set

# include folders
INC = -I../../include -I../../../raven-set

# folder for .o files
ODIR=./obj

OPT=-O2

# sources
_CIN = cintest.o 
CIN = $(patsubst %,$(ODIR)/%,$(_CIN))

_TIMER = timertest.o
TIMER = $(patsubst %,$(ODIR)/%,$(_TIMER))

_SERVER = tcpserver.o cTCP.o
SERVER = $(patsubst %,$(ODIR)/%,$(_SERVER))

_CLIENT = tcpclient.o cTCP.o
CLIENT = $(patsubst %,$(ODIR)/%,$(_CLIENT))

$(ODIR)/%.o: %.cpp 
	g++ $(OPT)  -std=c++17 $(INC) \
	-c -o $@ $<

cin: $(CIN)
	g++ $(OPT) \
	-o ../../bin/cintest.exe $^

timer: $(TIMER) 
	g++ $(OPT) \
	-o ../../bin/timertest.exe $^

server: $(SERVER)
	g++ -g  \
	-o ../../bin/tcpserver.exe $^ -lws2_32 -lwsock32

client: $(CLIENT)
	g++ -g  \
	-o ../../bin/tcpclient.exe $^ -lws2_32 -lwsock32

.PHONY: clean
clean:
	del /q .\obj


