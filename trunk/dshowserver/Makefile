include ../config.mak


CFLAGS=-I../loader -Wdisabled-optimization -Wno-pointer-sign -Wdeclaration-after-statement -I. -Wall -Wno-switch -Wpointer-arith -Wredundant-decls -pipe -ffast-math -fomit-frame-pointer -D_LARGEFILE_SOURCE -D__WINE__ -DNOAVIFILE_HEADERS -DMPLAYER
CFLAGS+=-fno-omit-frame-pointer -mno-omit-leaf-frame-pointer

ifdef DEBUG
  CFLAGS+= -g
  LOADER_OPT = "DEBUG=1"
else
  CFLAGS+= -O2
endif
ifdef STATIC
  STATICBUILD= -static
endif

WIN32_LIBS = -lole32 -lws2_32
LIBS = -lm

ifdef X_COMPILE
  LIBS += -lole32 -loleaut32 -luser32 -lws2_32
else
  ifdef WINELIB
    LIBS += -m32 -lole32 -loleaut32 -lrt -lpthread
  else
    LIBS += -lpthread -lrt
    REGISTERCODEC = registercodec
  endif
endif

LIBS += $(EXTRA_LIBS)

.PHONY: loader

all: $(OBJDIR) dshowserver $(REGISTERCODEC)

dshowserver: $(OBJDIR)/server.o $(OBJDIR)/timeout_sem.o $(OBJDIR)/defaults.o $(OBJDIR)/crc32.o $(OBJDIR)/libloader.a
	$(CC) $(STATICBUILD) -o $@ $^ $(LIBS)
ifdef X_COMPILE
	mv $@ $@.exe
	echo "#!/bin/sh" > $@
	echo "echo 'Starting wine dshowserver.exe'" >> $@
	echo 'wine ./dshowserver.exe $$*' >> $@
	chmod +x dshowserver
endif

registercodec: ../loader/modify_reg.c ../loader/registry.c
	$(CC) $(STATICBUILD) -I../loader -I../loader/compat -o $@ $<


$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/libloader.a: loader

loader:
	$(MAKE) -C ../loader $(LOADER_OPT)

$(OBJDIR)/%.o: %.c ../loader/dshow/DSVD_extern.h
	$(CC) $(CFLAGS) -o $@ -c $<

clean::
	rm -f ../objs-mingw/*
	rm -f ../objs-winelib/*
	rm -f ../objs/*

distclean:: clean
	rm -f ../objs-mingw/*
	rm -f ../objs-winelib/*
	rm -f ../objs/*
	rm -f dshowserver
	rm -f dshowserver.exe*

install::
	mkdir -p $(PREFIX_SHARE)
	mkdir -p $(PREFIX_EXE)
ifdef WINE_EXT
	cp dshowserver.$(WINE_EXT) $(PREFIX_SHARE)/
	echo "#!/bin/sh" > $(PREFIX_EXE)/dshowserver
	echo "echo 'Starting wine dshowserver.$(WINE_EXT)'" >> $(PREFIX_EXE)/dshowserver
	echo 'wine $(PREFIX_SHARE)/dshowserver.$(WINE_EXT) $$*' >> $(PREFIX_EXE)/dshowserver
	chmod +x $(PREFIX_EXE)/dshowserver
else
	cp dshowserver $(PREFIX_EXE)/
endif
