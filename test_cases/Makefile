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
	${RM} $(OBJ)  test_suite

tests: $(OBJ)
	$(CC) $(LINKOBJ) -g -o "test_suite" $(LIBS) -lrt

queue_test.o: queue_test.c
	$(CC) -c -g queue_test.c -o queue_test.o $(CFLAGS)

main_suite.o: main_suite.c
	$(CC) -c -g main_suite.c -o main_suite.o $(CFLAGS)

