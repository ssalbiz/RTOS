# Project: myRTX


CPP  = g++
CC   = gcc
MAKE = make
RES  = 
OBJ  = set.o  userProcs.o initialize.o iprocesses.o kernel.o queues.o userAPI.o startkbcrtc.o $(RES)
LINKOBJ  = userProcs.o initialize.o iprocesses.o kernel.o queues.o userAPI.o startkbcrtc.o set.o $(RES)
LIBS =   
INCS = include/
CXXINCS = 

CXXFLAGS = $(CXXINCS)  
CFLAGS = -I$(INCS) -g -Wall
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: myRTX CRT KB 

clean: 
	${RM} $(OBJ) myRTX CRT KB crt.o keyboard.o
	cd test_cases && $(MAKE) clean

myRTX: $(OBJ)
	$(CC) $(LINKOBJ) -o "myRTX" $(LIBS) -lrt

CRT: crt.o
	$(CC) $<  -o $@ -lrt

KB: keyboard.o
	$(CC) $<  -o $@ -lrt

userProcs.o: userProcs.c
	$(CC) -c  $< -o $@ $(CFLAGS)

initialize.o: initialize.c
	$(CC) -c  $< -o $@ $(CFLAGS)

iprocesses.o: iprocesses.c
	$(CC) -c  $< -o $@ $(CFLAGS)

kernel.o: kernel.c
	$(CC) -c  $< -o $@ $(CFLAGS)

queues.o: queues.c
	$(CC) -c  $< -o $@ $(CFLAGS)

userAPI.o: userAPI.c
	$(CC) -c  $< -o $@ $(CFLAGS)

startkbcrtc.o: startkbcrtc.c
	$(CC) -c  $< -o $@ $(CFLAGS)

crt.o: crt.c
	$(CC) -c  $< -o $@  $(CFLAGS)

keyboard.o: keyboard.c
	$(CC) -c  $< -o $@  $(CFLAGS)

set.o: set.s
	$(CC) -c $< -o $@

tests: myRTX CRT KB
	cp queues.o test_cases/
	@echo '---------------------------------------------------------------'
	@echo 'Running tests...'
	@echo '---------------------------------------------------------------'
	cd test_cases && $(MAKE) && ./tests.sh
	@echo '---------------------------------------------------------------'
	@echo 'Running memcheck on RTX'
	@echo '---------------------------------------------------------------'
	./tests.sh

