@ECHO OFF
Set MSSDK=D:\Program Files\Microsoft Platform SDK
Set VCDIR=D:\Program Files\Microsoft Visual C++ Toolkit 2003
Set LANGNAME=Czech
Set LANGID=0x405

::###################################::
Set PATH=%VCDIR%\bin;%MSSDK%\bin;%PATH%
Set INCLUDE=%VCDIR%\include;%MSSDK%\include;%INCLUDE%
Set LIB=%VCDIR%\lib;%MSSDK%\lib;%LIB%

rc /R /DAKELEDIT_STATICBUILD /DEXE_VERSION /DRC_VERSIONLANGID=%LANGID% /I "AkelEdit\Resources" /Fo"AkelPad.res" "AkelFiles\Langs\Resources\%LANGNAME%.rc"
cl /O1 /DAKELEDIT_STATICBUILD /DRC_VERSIONLANGID=%LANGID% AkelPad.cpp Edit.cpp AkelEdit\AkelEdit.cpp AkelPad.res /link kernel32.lib user32.lib gdi32.lib advapi32.lib shell32.lib comctl32.lib comdlg32.lib ole32.lib uuid.lib imm32.lib version.lib /SUBSYSTEM:WINDOWS /OPT:NOWIN98 /MACHINE:I386 /NODEFAULTLIB /ENTRY:_WinMain /OUT:AkelPad.exe

if exist AkelPad.obj del AkelPad.obj
if exist Edit.obj del Edit.obj
if exist AkelEdit.obj del AkelEdit.obj
if exist AkelPad.res del AkelPad.res
@PAUSE