@echo Running Code Patterns Txx...
@if exist Txx.txt del Txx.txt
@cm T01.exe > Txx.txt
@cm T02.exe >> Txx.txt
@cm T03.exe >> Txx.txt
@cm T04.exe >> Txx.txt
@cm T05.exe >> Txx.txt
@cm T06.exe >> Txx.txt
@cm T07.exe >> Txx.txt
@cm T08.exe >> Txx.txt
@cm T09.exe >> Txx.txt
@cm T10.exe >> Txx.txt
@cm T11.exe >> Txx.txt
@cm T12.exe >> Txx.txt
type Txx.txt
@echo on
@echo Checking test result...
@AUnit Txx.txt
pause
