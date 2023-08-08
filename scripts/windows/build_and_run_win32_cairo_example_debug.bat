@echo off

mingw32-make config=debug win32-cairo-example

echo Launching...

.\bin\Debug\win32-cairo-example.exe