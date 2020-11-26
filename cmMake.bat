@echo off
@echo. Pierre-Alexis Barras 2020. all rights reserved.
@echo. p_barras@encs.concordia.ca
@echo.
rem file compiles Cm supplied Cm code and generates a .exe
rem object (.o) files are then moved into their own directory in the active dir.

IF exist src\%1.c goto compile
IF "%1" == "" goto usage
IF NOT exist src\%1.c echo. ERROR: %1.c was not found in \src\
exit /b

:usage
echo. USAGE: cmMake.bat ^<name of main file w/o extension^> ?host
echo. host: specify that the target is the host (large flash + memory space)
echo. *Batch should be called at same level as the \src\ directory which contians the Cm VM source code.
echo.
echo. Example:
echo. "cmMake.bat admin host"
exit /b

:compile
IF exist objects\ ( echo >nul ) ELSE ( mkdir objects && echo \objects\ created )
@echo. Compiling Cm VM ...

gcc -c src\_console.c
IF "%2" == "host" ( gcc -c -D ConsoleOutputWithPrintf src\_cout.c ) ELSE ( gcc -c src\_cout.c )
gcc -c src\_xtoa.c
gcc -c src\hal.c
gcc -c src\ioreg.c
gcc -c src\out.c
gcc -c src\vm.c
gcc -c src\vmstack.c
gcc -c src\%1.c

gcc _console.o _cout.o _xtoa.o hal.o ioreg.o out.o vm.o vmstack.o %1.o -o cm.exe

@for /r "%~dp0" %%x in (*.o) do move "%%x" "%~dp0%objects" >nul
rem @move "src\cm.exe" . >nul

@echo. Sucessfully compiled Cm VM!