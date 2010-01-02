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
  LIBS += -lole32 -lws2_32
else
  ifdef WINELIB
    LIBS += -lole32 -lrt -lpthread
  else
    LIBS += -lpthread -lrt
    REGISTERCODEC = registercodec
  endif
endif

LIBS += $(EXTRA_LIBS)

.PHONY: loader

all: $(OBJDIR) dshowserver $(REGISTERCODEC)

dshowserver: $(OBJDIR)/server.o $(OBJDIR)/timeout_sem.o $(OBJDIR)/libloader.a
	$(CC) $(STATICBUILD) -o $@ $^ $(LIBS)
ifdef X_COMPILE
	mv $@ $@.exe
	ln -s $@.exe $@
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
	rm -f *.o *~
	$(MAKE) -C ../loader clean

distclean:: clean
	rm -f dshowserver
