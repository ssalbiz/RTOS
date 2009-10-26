#!/bin/bash

./main_test_suite

echo "------------------------------------"
echo "Running test suite through valgrind!"
echo "------------------------------------"


valgrind $@ --leak-check=full --show-reachable=yes ./main_test_suite 2>&1 
