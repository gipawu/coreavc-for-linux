ifndef X_COMPILE
  ifndef LOADER_WIN32
    WINELIB=1
  endif
endif

ifdef X_COMPILE
  AR = i586-mingw32msvc-ar
  RANLIB = i586-mingw32msvc-ranlib
  CC = i586-mingw32msvc-gcc
  LD = i586-mingw32msvc-ld
  OBJDIR = ../objs-mingw
  WINE_EXT = exe
endif

ifdef WINELIB
  CC = winegcc
  OBJDIR = ../objs-winelib
  WINE_EXT = exe.so
endif

AR ?= ar
RANLIB ?= ranlib
OBJDIR ?= ../objs

PREFIX ?= /usr/local
PREFIX_EXE ?= $(PREFIX)/bin
PREFIX_SHARE ?= $(PREFIX)/share/dshowserver
