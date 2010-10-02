@ECHO OFF
Set MSSDK=D:\Program Files\Microsoft Platform SDK
Set VCDIR=D:\Program Files\Microsoft Visual C++ Toolkit 2003
Set LANGNAME=Hungarian
Set LANGID=0x40E

::###################################::
Set PATH=%VCDIR%\bin;%MSSDK%\bin;%PATH%
Set INCLUDE=%VCDIR%\include;%MSSDK%\include;%INCLUDE%
Set LIB=%VCDIR%\lib;%MSSDK%\lib;%LIB%

rc /R /DDLL_VERSION /DRC_VERSIONLANGID=%LANGID% /I "..\..\AkelEdit\Resources" /Fo"%LANGNAME%.res" "Resources\%LANGNAME%.rc"
cl /O1 Module.cpp /LD /link "%LANGNAME%.res" /OPT:NOWIN98 /MACHINE:I386 /NODEFAULTLIB /ENTRY:DllMain /OUT:"%LANGNAME%.dll"

if exist Module.obj del Module.obj
if exist "%LANGNAME%.res" del "%LANGNAME%.res"
@PAUSE