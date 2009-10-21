# Project: myRTX


CPP  = g++
CC   = gcc

RES  = 
OBJ  = set.o  userProcs.o initialize.o iprocesses.o kernel.o queues.o userAPI.o startkbcrtc.o $(RES)
LINKOBJ  = userProcs.o initialize.o iprocesses.o kernel.o queues.o userAPI.o startkbcrtc.o set.o $(RES)
LIBS =   
INCS = -I include/
CXXINCS = 

CXXFLAGS = $(CXXINCS)  
CFLAGS = $(INCS) -g 
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: myRTX CRT KB

clean: 
	${RM} $(OBJ) myRTX CRT KB crt.o keyboard.o

myRTX: $(OBJ)
	$(CC) $(LINKOBJ) -g -o "myRTX" $(LIBS) -lrt

CRT: crt.o
	$(CC) crt.o -g -o "CRT" -lrt

KB: keyboard.o
	$(CC) keyboard.o -g -o "KB" -lrt

userProcs.o: userProcs.c
	$(CC) -c -g userProcs.c -o userProcs.o $(CFLAGS)

initialize.o: initialize.c
	$(CC) -c -g initialize.c -o initialize.o $(CFLAGS)

iprocesses.o: iprocesses.c
	$(CC) -c -g iprocesses.c -o iprocesses.o $(CFLAGS)

kernel.o: kernel.c
	$(CC) -c -g kernel.c -o kernel.o $(CFLAGS)

queues.o: queues.c
	$(CC) -c -g queues.c -o queues.o $(CFLAGS)

userAPI.o: userAPI.c
	$(CC) -c -g userAPI.c -o userAPI.o $(CFLAGS)

startkbcrtc.o: startkbcrtc.c
	$(CC) -c -g startkbcrtc.c -o startkbcrtc.o $(CFLAGS)

crt.o: crt.c
	$(CC) -c -g crt.c -o crt.o $(CFLAGS)

keyboard.o: keyboard.c
	$(CC) -c -g keyboard.c -o keyboard.o $(CFLAGS)

set.o: set.s
	$(CC) -c set.s -o set.o

