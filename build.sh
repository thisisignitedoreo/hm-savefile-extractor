#!/bin/sh
set -xe
gcc -mwindows -o saveunpack saveunpack.c
gcc -mwindows -o savepack savepack.c