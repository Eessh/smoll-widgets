@echo off

echo Cleaning executables ...
del /q bin\Debug\*.exe >nul 2>&1
del /q bin\Release\*.exe >nul 2>&1

echo Cleaning built libraries ...
del /q bin\Debug\*.lib >nul 2>&1
del /q bin\Release\*.lib >nul 2>&1

echo Cleaning object files ...
del /q /s obj\Debug\* >nul 2>&1
del /q /s obj\Release\* >nul 2>&1

echo Cleaning misc files ...
del /q Makefile >nul 2>&1
del /q *.make >nul 2>&1

echo Done.

exit
