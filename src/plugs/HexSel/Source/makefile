# Make file for build plugin DLL with GCC
# mingw32-make.exe all clean

ifndef BIT
  BIT = 32
endif
ifndef NIX
  DEL = del
else
  DEL = rm -f
endif

ifeq ($(BIT), 64)
  CC = x86_64-w64-mingw32-g++.exe
  RES = windres.exe
  STRIP = strip.exe
else
  CC = g++.exe
  RES = windres.exe
  STRIP = strip.exe
endif
CFLAGS = -Os -g0 -Werror -Wchar-subscripts -Wcomment -Wformat -Wreturn-type -Wsign-compare -Wswitch -Wtrigraphs -Wuninitialized -Wunknown-pragmas -Wunused-function -Wunused-label -Wunused-value -Wunused-variable
LDFLAGS = -shared -static-libgcc -lkernel32 -luser32 -lgdi32
DEFINES = -D CINTERFACE

all: HexSel.dll

HexSel.dll: HexSel.o Resource.o
	$(CC) $(CFLAGS) $(DEFINES) HexSel.o Resource.o $(LDFLAGS) -o HexSel.dll
	$(STRIP) --strip-all HexSel.dll

HexSel.o:
	$(CC) $(CFLAGS) $(DEFINES) -c -xc HexSel.c

Resource.o:
	$(RES) --use-temp-file -D RC_VERSIONBIT=$(BIT) -I Resources Resources/HexSel.rc -o Resource.o

clean:
	$(DEL) HexSel.o
	$(DEL) Resource.o
