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
LDFLAGS = -shared -static-libgcc -lkernel32 -luser32 -lgdi32 -lcomctl32 -ladvapi32 -lole32 -lshell32 -luuid
DEFINES = -D CINTERFACE

all: ContextMenu.dll

ContextMenu.dll: ContextMenu.o Resource.o
	$(CC) $(CFLAGS) $(DEFINES) ContextMenu.o Resource.o $(LDFLAGS) -o ContextMenu.dll
	$(STRIP) --strip-all ContextMenu.dll

ContextMenu.o:
	$(CC) $(CFLAGS) $(DEFINES) -c -xc ContextMenu.c

Resource.o:
	$(RES) --use-temp-file -D RC_VERSIONBIT=$(BIT) -I Resources Resources/ContextMenu.rc -o Resource.o

clean:
	$(DEL) ContextMenu.o
	$(DEL) Resource.o
