# Project: myRTX


CPP  = g++
CC   = gcc

RES  = 
OBJ  = queue_test.o main_suite.o
LINKOBJ  = $(OBJ)
LIBS =   
INCS = -I ../include/
CXXINCS = 

CXXFLAGS = $(CXXINCS)  
CFLAGS = $(INCS) -g -Wall
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: tests

clean: 
	${RM} $(OBJ)  main_test_suite

clean-obj:
	${RM} ${OBJ}

tests: $(OBJ)
	$(CC) $(LINKOBJ) -g -o "main_test_suite" $(LIBS) -lrt

queue_test.o: queue_test.c queue_test.h
	$(CC) -c -g queue_test.c -o queue_test.o $(CFLAGS)

main_suite.o: main_suite.c
	$(CC) -c -g main_suite.c -o main_suite.o $(CFLAGS)

