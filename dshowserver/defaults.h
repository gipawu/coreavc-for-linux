#ifndef _DEFUALTS_H_
#define _DEFUALTS_H_
enum {
    PARAM_WIDTH,
    PARAM_HEIGHT,
    PARAM_FOURCC,
    PARAM_OUTFMT,
    PARAM_OUTBITS,
    PARAM_GUID
};

void get_default(const char *str, int param_num, void *param);

#endif
