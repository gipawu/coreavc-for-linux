//#define MSG_USE_COLORS
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <pwd.h>

#include "cpudetect.h"

extern char **environ;
//get_path
char *get_path(const char *filename){
    char* pthn, *dir=NULL;
    int i;
    for (i = 0; environ[i] != NULL; i++) {
      if(strncmp("HOME=", environ[i], 5) == 0) {
        dir = environ[i] + 5;
        break;
      }
    }
    if(dir == NULL) {
      fprintf(stderr, "Couldn't find HOME dir\n");
      exit(1);
    }
    asprintf(&pthn, "%s/.mplayer/registry32", dir);
    return pthn;
}

//mp_msg
extern unsigned int print_verbose_messages;

#define VB_PLAYBACK 0x00000008

int mp_msg_test(int mod, int lev)
{
    return ((print_verbose_messages & VB_PLAYBACK) == VB_PLAYBACK);
}

#define MSGSIZE_MAX 2048
void mp_msg(int mod, int lev, const char *format, ... ){
    va_list va;
    char tmp[MSGSIZE_MAX];
    if (! mp_msg_test(mod, lev))
        return;
    va_start(va, format);
    vsnprintf(tmp, MSGSIZE_MAX, format, va);
    va_end(va);
    tmp[MSGSIZE_MAX-2] = '\n';
    tmp[MSGSIZE_MAX-1] = 0;
    printf(tmp);
}

//cpudetect
CpuCaps gCpuCaps = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
