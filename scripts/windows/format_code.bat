@echo off

echo Formatting ...

clang-format -i -style=file include/*.h include/widgets/*.h src/*.c src/widgets/*.c

echo Done.

exit

