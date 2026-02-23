#!/bin/bash
gcc src/main.c -o main -std=c99 -Wall -Werror -lraylib -lGL -lm -lpthread -ldl -lrt -lX11