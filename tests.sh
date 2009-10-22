#!/bin/bash

valgrind $@ --leak-check=full --show-reachable=yes --track-origins=yes ./myRTX 2>&1 
