# Project: myRTX


CPP  = g++
CC   = gcc
MAKE = make
RES  = 
OBJ  =  userProcs.o initialize.o iprocesses.o kernel.o queues.o userAPI.o $(RES)
LINKOBJ  = userProcs.o initialize.o iprocesses.o kernel.o queues.o userAPI.o $(RES)
LIBS =  -lcurses 
INCS = include/
CXXINCS = 

CXXFLAGS = $(CXXINCS)
ifndef DEBUG
  CFLAGS = -I$(INCS) -g -Wall
else
  CFLAGS = -I$(INCS) -g -Wall -DDEBUG=$(DEBUG)
endif
RM = rm -f
DOCS = doxygen

.PHONY: all all-before all-after clean clean-obj

all: myRTX CRT KB 

docs: Doxyfile
	$(DOCS)

clean: 
	$(RM) $(OBJ) myRTX CRT KB crt.o keyboard.o
	cd test_cases && $(MAKE) clean

clean-obj:
	$(RM) $(OBJ) crt.o keyboard.o
	cd test_cases && $(MAKE) clean-obj

myRTX: $(OBJ)
	$(CC) $(LINKOBJ) -o "myRTX" $(LIBS) -lrt

CRT: crt.o
	$(CC) $<  -o $@ -lrt $(LIBS)

KB: keyboard.o
	$(CC) $<  -o $@ -lrt $(LIBS)

userProcs.o: userProcs.c include/userProcs.h include/global.h
	$(CC) -c  $< -o $@ $(CFLAGS)

initialize.o: initialize.c include/initialize.h include/global.h
	$(CC) -c  $< -o $@ $(CFLAGS)

iprocesses.o: iprocesses.c include/iprocesses.h include/global.h
	$(CC) -c  $< -o $@ $(CFLAGS)

kernel.o: kernel.c include/kernel.h include/global.h
	$(CC) -c  $< -o $@ $(CFLAGS)

queues.o: queues.c include/queues.h include/global.h
	$(CC) -c  $< -o $@ $(CFLAGS)

userAPI.o: userAPI.c include/userAPI.h include/global.h
	$(CC) -c  $< -o $@ $(CFLAGS)

crt.o: crt.c include/global.h
	$(CC) -c  $< -o $@  $(CFLAGS)

keyboard.o: keyboard.c include/global.h
	$(CC) -c  $< -o $@  $(CFLAGS)


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

