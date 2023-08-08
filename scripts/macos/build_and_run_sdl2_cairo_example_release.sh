#!/bin/bash

echo Building...
make config=release sdl2-cairo-example

echo Launching...
./bin/Release/sdl2-cairo-example