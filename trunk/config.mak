ifdef X_COMPILE
  AR = i586-mingw32msvc-ar
  RANLIB = i586-mingw32msvc-ranlib
  CC = i586-mingw32msvc-gcc
  LD = i586-mingw32msvc-ld
  OBJDIR = ../objs-mingw
endif

ifdef WINELIB
  CC = winegcc
  OBJDIR = ../objs-winelib
endif

AR ?= ar
RANLIB ?= ranlib
OBJDIR ?= ../objs

