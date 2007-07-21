
#ifndef _MP_MSG_H
#define _MP_MSG_H

// defined in mplayer.c and mencoder.c
extern int verbose;
// verbosity elevel:

/* Only messages level MSGL_FATAL-MSGL_STATUS should be translated,
 * messages level MSGL_V and above should not be translated. */

#define MSGL_FATAL 0  // will exit/abort
#define MSGL_ERR 1    // continues
#define MSGL_WARN 2   // only warning
#define MSGL_HINT 3   // short help message
#define MSGL_INFO 4   // -quiet
#define MSGL_STATUS 5 // v=0
#define MSGL_V 6      // v=1
#define MSGL_DBG2 7   // v=2
#define MSGL_DBG3 8   // v=3
#define MSGL_DBG4 9   // v=4
#define MSGL_DBG5 10  // v=5

#define MSGL_FIXME 1  // for conversions from printf where the appropriate MSGL is not known; set equal to ERR for obtrusiveness
#define MSGT_FIXME 0  // for conversions from printf where the appropriate MSGT is not known; set equal to GLOBAL for obtrusiveness

// code/module:
#define MSGT_WIN32 15	// win32 dll stuff
#define MSGT_LOADER 44 // dll loader messages
#define MSGT_MAX 64

void mp_msg_init(void);
int mp_msg_test(int mod, int lev);

#include "config.h"

#ifdef TARGET_OS2
// va_start/vsnprintf seems to be broken under OS2 :(
#define mp_msg(mod,lev, fmt, args... ) do{if((lev)<=mp_msg_levels[mod]) printf( fmt, ## args );}while(0)
#define mp_dbg(mod,lev, args... ) 
#else

#ifdef __GNUC__
void mp_msg(int mod, int lev, const char *format, ... ) __attribute__ ((format (printf, 3, 4)));
#   ifdef MP_DEBUG
#      define mp_dbg(mod,lev, args... ) mp_msg(mod, lev, ## args )
#   else
#      define mp_dbg(mod,lev, args... ) /* only usefull for developers */
#   endif
#else // not GNU C
void mp_msg(int mod, int lev, const char *format, ... );
#   ifdef MP_DEBUG
#      define mp_dbg(mod,lev, ... ) mp_msg(mod, lev, __VA_ARGS__)
#   else
#      define mp_dbg(mod,lev, ... ) /* only usefull for developers */
#   endif
#endif

const char* filename_recode(const char* filename);

#endif
#endif
