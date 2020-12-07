@echo off
@echo. Pierre-Alexis Barras 2020. all rights reserved.
@echo. p_barras@encs.concordia.ca
@echo.
rem file compiles Cm supplied Cm code and generates a .exe or oplads a .hx to a target via avr
rem object (.o) files are then moved into their own directory in the active dir.

rem goto debug
IF "%1" == "" goto usage
IF exist src\%1.c goto compile_type
IF NOT exist src\%1.c 

:usage
echo. USAGE: cmMake.bat ^<name of main file w/o extension^> host/COM# h?
echo. host: specify that the target is the host (large flash + memory space)
echo. COM#: specify to upload compilation unto target via this COM port.
echo. h: specify to compile for 'printf' as output function
echo. *Batch should be called at same level as the \src\ directory which contians the Cm VM source code.
echo.
echo. Examples:
echo. "cmMake.bat admin host"
echo. "cmMake.bat admin0_quickTest COM3"
exit /b

:compile_type
@echo. Compiling Cm VM ...
IF exist build\ ( echo >nul ) ELSE ( mkdir build && echo \build\ created )
IF "%2" == "" goto error_compilenotdef
IF "%2" == "host" goto host_compile

:avr_compile
avr-gcc -Os -Wall -DF_CPU=16000000UL -mmcu=atmega328p src\%1.c src\vmstack.c src\hal.c src\hal_Out.c src\bsl_COut.c src\bsl_xtoa.c src\vm.c src\bsl_interman.c src\hal_interman.c src\bsl_IOReg.c -o cm.o
IF %errorlevel% neq 0 exit /b %errorlevel%
avr-objcopy -O ihex -j .text -j .data cm.o  cm.hex
IF %errorlevel% neq 0 exit /b %errorlevel%
avrdude -c arduino -p atmega328p -b 57600 -P %2 -D -Uflash:w:cm.hex:i
IF %errorlevel% neq 0 exit /b %errorlevel%
move "cm.o" build\
goto done

:host_compile
gcc -c src\*
IF %errorlevel% neq 0 goto error_cleanup
IF "%3" == "h" ( gcc -c -D ConsoleOutputWithPrintf src\_cout.c ) ELSE ( gcc -c src\_cout.c )
IF %errorlevel% neq 0 exit /b %errorlevel%
gcc -c src\%1.c
IF %errorlevel% neq 0 exit /b %errorlevel%
gcc _console.o _cout.o _xtoa.o hal.o ioreg.o out.o vm.o vmstack.o %1.o -o cm.exe
IF %errorlevel% neq 0 exit /b %errorlevel%
@for /r "%~dp0" %%x in (*.o) do move "%%x" "%~dp0%build" >nul
@for /r "%~dp0" %%x in (*.h.gch) do move "%%x" "%~dp0%build" >nul
goto done

rem error section
:error_filenotfound
echo. ERROR: %1.c was not found in \src\
exit /b

:error_compilenotdef
echo. ERROR: host/target compilation type not specified.
exit /b

:error_cleanup
@for /r "%~dp0" %%x in (*.o) do move "%%x" "%~dp0%build" >nul
@for /r "%~dp0" %%x in (*.h.gch) do move "%%x" "%~dp0%build" >nul
exit /b

:done
@echo. ... Done.
exit /b

:debug
echo. ***Issues exist in this version. Will continue to work on solution.***
exit /b