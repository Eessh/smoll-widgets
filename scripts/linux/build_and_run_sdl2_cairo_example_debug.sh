#!/bin/bash

echo Building...
make config=debug sdl2-cairo-example

echo Launching...
./bin/Debug/sdl2-cairo-example