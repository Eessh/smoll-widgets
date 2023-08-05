@echo off

mingw32-make config=debug

echo Launching...

.\bin\Debug\sdl2-cairo-example.exe

rem echo:
rem echo Launching...
rem .\bin\Debug\smoll-widgets.exe