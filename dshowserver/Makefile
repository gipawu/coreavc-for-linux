CFLAGS=-I../loader -Wdisabled-optimization -Wno-pointer-sign -Wdeclaration-after-statement -I. -Wall -Wno-switch -Wpointer-arith -Wredundant-decls -pipe -ffast-math -fomit-frame-pointer -D_LARGEFILE_SOURCE -D__WINE__ -DNOAVIFILE_HEADERS -DMPLAYER
CFLAGS+=-fno-omit-frame-pointer -mno-omit-leaf-frame-pointer

ifdef DEBUG
  CFLAGS+= -g
  LOADER_OPT = "DEBUG=1"
else
  CFLAGS+= -O2
endif

all: dshowserver

dshowserver: server.o ../loader/libloader.a
	$(CC) -o $@ -static $^ $(EXTRA_LIBS) -lpthread -lrt -lm

../loader/libloader.a:
	$(MAKE) -C ../loader $(LOADER_OPT)

clean::
	rm -f *.o *~
	$(MAKE) -C ../loader clean

distclean:: clean
	rm -f dshowserver
