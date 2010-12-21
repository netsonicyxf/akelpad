@ECHO OFF
Set MSSDK=C:\Program Files\Microsoft Platform SDK
Set LANGNAME=Chinese (Traditional)
Set LANGID=0x404
Set BIT=64

::###################################::
Set PATH=%MSSDK%\bin\win64\x86\AMD64;%MSSDK%\bin;%PATH%
Set INCLUDE=%MSSDK%\include;%MSSDK%\include\crt;%INCLUDE%
Set LIB=%MSSDK%\lib\AMD64;%LIB%

rc /R /DDLL_VERSION /DRC_VERSIONLANGID=%LANGID% /DRC_VERSIONBIT=%BIT% /I "..\..\AkelEdit\Resources" /Fo"%LANGNAME%.res" "Resources\%LANGNAME%.rc"
cl /O1 /GS- Module.c /LD /link "%LANGNAME%.res" /OPT:NOWIN98 /MACHINE:AMD64 /NODEFAULTLIB /ENTRY:DllMain /OUT:"%LANGNAME%.dll"

if exist Module.obj del Module.obj
if exist "%LANGNAME%.res" del "%LANGNAME%.res"
@PAUSE