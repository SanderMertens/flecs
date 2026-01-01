#!/usr/bin/env bash
set -ex

gcc-15 -c ../../distr/flecs.c -std=c99 -O2 -o flecs.o
g++-15 repro.cpp flecs.o -o repro -std=c++17 -lpthread
