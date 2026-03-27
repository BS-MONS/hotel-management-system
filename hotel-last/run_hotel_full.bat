@echo off
REM Batch script to compile and run the full hotel project (C++)

REM Set the source and output file names
set SRC=main.cpp
set OUT=hotel.exe

REM Compile the project (add all .cpp and .c files if needed)
g++ %SRC% admin.c client.c login.c -o %OUT%
if errorlevel 1 (
    echo Compilation failed.
    exit /b 1
)

REM Run the executable
%OUT%
