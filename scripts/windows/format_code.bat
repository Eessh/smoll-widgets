@echo off

echo Formatting ...

clang-format -i -style=file ^
include/*.h ^
include/widgets/*.h ^
src/*.c ^
src/widgets/*.c ^
backends/sdl2_cairo/*.h ^
backends/sdl2_cairo/*.c ^
backends/win32_cairo/*.h ^
backends/win32_cairo/*.c

echo Done.
exit
