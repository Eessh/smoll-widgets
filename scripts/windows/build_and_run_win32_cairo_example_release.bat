@echo off

mingw32-make config=release win32-cairo-example

echo Launching...

.\bin\Release\win32-cairo-example.exe