#ifndef MPLAYER_CONFIG_H
#define MPLAYER_CONFIG_H 1

//#include "../../config.h"
/* enables / disables QTX codecs */
#define USE_QTX_CODECS 1
/* Define this if your system has the "malloc.h" header file */
#define HAVE_MALLOC_H 1
/* Define this if your system has the "sys/mman.h" header file */
#define HAVE_SYS_MMAN_H 1
/* Define this if you have the elf dynamic linker -ldl library */
#define HAVE_LIBDL 1
/* Define this if your system has vsscanf */
#define HAVE_VSSCANF 1
/* Build our Win32-loader */
#define WIN32_LOADER 1
/* Win32 DLL support */
#define USE_WIN32DLL 1
#define WIN32_PATH "/usr/lib/win32"
/* DirectShow support */
#define USE_DIRECTSHOW 1

#ifdef DEBUG
#define TRACE printf
#define dbg_printf printf
#else
#define TRACE(...)
#define dbg_printf(...)
#endif

#endif /* MPLAYER_CONFIG_H */
