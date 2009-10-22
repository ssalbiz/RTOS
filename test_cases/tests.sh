#!/bin/bash

valgrind $@ --leak-check=full --show-reachable=yes --track-origins=yes ./test_suite 2>&1 
