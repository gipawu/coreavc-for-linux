LIBNAME_COMMON = libloader.a

#CFLAGS+=-Ddbg_printf=__vprintf -DTRACE=__vprintf -DDETAILED_OUT

SRCS_COMMON  = driver.c afl.c vfl.c
SRCS_COMMON += ldt_keeper.c pe_image.c module.c ext.c win32.c \
               pe_resource.c resource.c registry.c elfdll.c
SRCS_COMMON += wrapper.S
SRCS_COMMON += compat/compat.c
SRCS_COMMON += osdep/mmap_anon.c
SRCS_COMMON += dshow/DS_Filter.c \
               dshow/DS_VideoDecoder.c \
               dshow/allocator.c \
               dshow/mediatype.c \
               dshow/cmediasample.c \
               dshow/guids.c \
               dshow/inputpin.c \
               dshow/outputpin.c
OBJS_COMMON    += $(addsuffix .o, $(basename $(SRCS_COMMON)) )

CFLAGS=-Icompat -Idshow -Wdisabled-optimization -Wno-pointer-sign -Wdeclaration-after-statement -I. -Wall -Wno-switch -Wpointer-arith -Wredundant-decls -O0 -pipe -ffast-math -fomit-frame-pointer -D_LARGEFILE_SOURCE
CFLAGS+=-fno-omit-frame-pointer -mno-omit-leaf-frame-pointer -g

all: $(LIBNAME_COMMON) registercodec

registercodec: modify_reg.c registry.c
	$(CC) $(CFLAGS) -o $@ -static $<

$(LIBNAME_COMMON):   $(OBJS_COMMON)
	ar r $@ $^
	ranlib $@

clean::
	rm -f dshow/*.o dshow/*~
	rm -f compat/*.o compat/*~
	rm -f osdep/*.o osdep/*~
	rm -f *.o *.a *~

distclean:: clean
	rm -f registercodec
