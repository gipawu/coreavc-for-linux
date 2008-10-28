LIBNAME_COMMON = libloader.a

include ../config.mak

ifdef X_COMPILE
  WIN32_EMULATION = no
  SHARED_MEM = no
else
  ifdef WINELIB
    WIN32_EMULATION = no
    CFLAGS += -D__MINGW32__
  endif
endif

SHARED_MEM      ?= yes
WIN32_EMULATION ?= yes

#CFLAGS+=-Ddbg_printf=__vprintf -DTRACE=__vprintf -DDETAILED_OUT
CFLAGS-$(WIN32_EMULATION) += -DWIN32_LOADER=1
CFLAGS-$(SHARED_MEM) += -DUSE_SHARED_MEM

SRCS_COMMON += dshow/DS_Filter.c \
               dshow/DS_VideoDecoder.c \
               dshow/allocator.c \
               dshow/mediatype.c \
               dshow/cmediasample.c \
               dshow/guids.c \
               dshow/inputpin.c \
               dshow/outputpin.c
SRCS_COMMON-$(WIN32_EMULATION) += \
               driver.c afl.c vfl.c \
               ldt_keeper.c pe_image.c module.c ext.c win32.c \
               pe_resource.c resource.c registry.c elfdll.c \
               wrapper.S \
               compat/compat.c \
               osdep/mmap_anon.c

SRCS_COMMON   += $(SRCS_COMMON-yes)

OBJS_COMMON   = $(addprefix $(OBJDIR)/, \
                  $(addsuffix .o, $(notdir $(basename $(SRCS_COMMON)) ) ) )

INC_COMMON := $(shell ls *.h */*.h)

ifdef DEBUG
  CFLAGS+=-g
else
  CFLAGS+=-O2
endif
CFLAGS+=-Icompat -Idshow -Wdisabled-optimization -Wno-pointer-sign -Wdeclaration-after-statement -I. -Wall -Wno-switch -Wpointer-arith -Wredundant-decls -pipe -ffast-math -fomit-frame-pointer -D_LARGEFILE_SOURCE
CFLAGS+=-fno-omit-frame-pointer -mno-omit-leaf-frame-pointer
CFLAGS+= $(CFLAGS-yes)

all: $(OBJDIR) $(OBJDIR)/$(LIBNAME_COMMON)

$(OBJDIR)/$(LIBNAME_COMMON):   $(OBJS_COMMON)
	$(AR) r $@ $^
	$(RANLIB) $@

$(OBJDIR):
	mkdir $(OBJDIR)

clean::
	rm -f dshow/*.o dshow/*~
	rm -f compat/*.o compat/*~
	rm -f osdep/*.o osdep/*~
	rm -f *.o ../objs*/*.o
	rm -f ../objs*/$(LIBNAME_COMMON) $(LIBNAME_COMMON).a
	rm -f *~

distclean:: clean
	rm -f registercodec

$(OBJDIR)/%.o: */%.c $(INC_COMMON)
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/%.o: %.c $(INC_COMMON)
	$(CC) $(CFLAGS) -o $@ -c $<

$(OBJDIR)/%.o: %.S
	$(AS) -o $@ $<
