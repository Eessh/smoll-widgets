#!/bin/bash

echo Formatting...
clang-format -i -style=file include/**.h src/**.c backends/sdl2_cairo/*.h backends/sdl2_cairo/*.c

echo Done.