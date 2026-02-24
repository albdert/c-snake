#!/bin/bash
gcc src/main.c -o snake -std=c99 -Wall -Werror -L./lib/libraylib.a -lraylib -lGL -lm -lpthread -ldl -lrt -lX11