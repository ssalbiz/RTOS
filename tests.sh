#!/bin/bash

valgrind $@ --leak-check=full --show-reachable=yes ./myRTX 2>&1 
