CFLAGS=-I../loader -Wdisabled-optimization -Wno-pointer-sign -Wdeclaration-after-statement -I. -Wall -Wno-switch -Wpointer-arith -Wredundant-decls -pipe -ffast-math -fomit-frame-pointer -D_LARGEFILE_SOURCE -D__WINE__ -DNOAVIFILE_HEADERS -DMPLAYER
CFLAGS+=-fno-omit-frame-pointer -mno-omit-leaf-frame-pointer
CFLAGS+= -O2

all: dshowserver

dshowserver: server.o ../loader/libloader.a
	$(CC) -o $@ -static $^ $(EXTRA_LIBS) -lpthread -lrt -lm

../loader/libloader.a:
	$(MAKE) -C ../loader

clean::
	rm -f *.o *~

distclean:: clean
	rm -f dshowserver
